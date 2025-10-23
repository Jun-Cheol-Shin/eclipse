// Fill out your copyright notice in the Description page of Project Settings.


#include "EpDropItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "NiagaraComponent.h"
#include "InputAction.h"

#include "../PlayerCore/EpPlayerController.h"
#include "../PlayerCore/Component/Inventory/EpInventoryComponent.h"
#include "../PlayerCore/Component/Inventory/EclipseInventoryItem.h"

#include "../Option/EpInputConfig.h"

#include "../PlayerCore/EpPlayerState.h"

#include "../GameModes/EpGameInstance.h"

#include "../Subsystems/EpInputManagerSubSystem.h"
#include "../Subsystems/EpUIManagerSubsystem.h"
#include "../Subsystems/EpResourceSubSystem.h"
#include "../UI/Game/Interact/InteractPrompt.h"

void AEpDropItemActor::Set(UEclipseInventoryItem* InItem)
{
	// TODO : 
	// Server에서 실행시키자. SetOwner() 필수
	// 액터 인스턴스는 Pooling할 예정..

	ItemData = InItem;
	
	if (ItemData.IsValid())
	{
		EItemRarity RarityEnum = ItemData->GetRarity();
		Particle->SetColorParameter(TEXT("GunPad_Color"), EPColorPalette(GetWorld()).GetColor(RarityEnum));

		// BP_Sword에 여러 Sword mesh가 있다면?
		FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();

		TSharedPtr<FStreamableHandle> Handle = StreamManager.RequestAsyncLoad(ItemData->GetMeshPath(),
			FStreamableDelegate::CreateWeakLambda(this, [WeakOuter = TWeakObjectPtr<AEpDropItemActor>(this), Path = ItemData->GetMeshPath()]()
				{
					if (WeakOuter.IsValid() && nullptr != WeakOuter->Mesh)
					{
						WeakOuter->Mesh->SetStaticMesh(Cast<UStaticMesh>(Path.ResolveObject()));
					}

					else
					{
						UE_LOG(LogTemp, Error, TEXT("Invalid DropItemActor. %s"), ANSI_TO_TCHAR(__FUNCTION__));
					}

				}));
	}
}

void AEpDropItemActor::Reset()
{
	ItemData.Reset();

	Trigger->OnComponentBeginOverlap.RemoveAll(this);
	Trigger->OnComponentEndOverlap.RemoveAll(this);
}

void AEpDropItemActor::OnInteract(APlayerController* InOwningController)
{
	if (ensure(ItemData.IsValid() && InOwningController))
	{	
		if (AEpPlayerState* PlayerState = InOwningController->GetPlayerState<AEpPlayerState>())
		{
			if (UEpInventoryComponent* InventoryComponent = PlayerState->GetInventoryComponent())
			{
				// client to server
				InventoryComponent->Server_AddItem(ItemData.Get());
			}

			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid InventoryComponent. %s"), ANSI_TO_TCHAR(__FUNCTION__));
			}
		}

		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Player State. %s"), ANSI_TO_TCHAR(__FUNCTION__));
		}
	}
}

void AEpDropItemActor::OnPing()
{
	UE_LOG(LogTemp, Display, TEXT("Ping!"));
}

void AEpDropItemActor::OnUseDirect()
{
	UE_LOG(LogTemp, Display, TEXT("Use Direct!"));
}

void AEpDropItemActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ENetMode::NM_DedicatedServer == GetNetMode())
	{
		return;
	}


	// Show Prompt
	UE_LOG(LogTemp, Display, TEXT("Show Prompt"));

	UEpGameInstance* GameInst = Cast<UEpGameInstance>(GetGameInstance());
	if (!ensure(GameInst))
	{
		return;
	}

	UEpUIManagerSubsystem* UISubSystem = GameInst->GetSubsystem<UEpUIManagerSubsystem>();
	if (ensure(UISubSystem))
	{
		UISubSystem->ShowLayerWidget<UInteractPrompt>(FOnCompleteLoadedWidgetSignature::CreateWeakLambda(this, [WeakOwner = TWeakObjectPtr(this)](UCommonActivatableWidget* InPrompt)
			{
				UInteractPrompt* Prompt = Cast<UInteractPrompt>(InPrompt);
				if (Prompt && WeakOwner.IsValid())
				{
					Prompt->Set(WeakOwner.Get());
				}
			}));
	}

}

void AEpDropItemActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ENetMode::NM_DedicatedServer == GetNetMode())
	{
		return;
	}

	// Hide Prompt
	UE_LOG(LogTemp, Display, TEXT("Hide Prompt"));

	UEpGameInstance* GameInst = Cast<UEpGameInstance>(GetGameInstance());
	if (!ensure(GameInst))
	{
		return;
	}

	UEpUIManagerSubsystem* UISubSystem = GameInst->GetSubsystem<UEpUIManagerSubsystem>();
	if (ensure(UISubSystem))
	{
		UISubSystem->HideLayerWidget<UInteractPrompt>();
	}
}

// Sets default values
AEpDropItemActor::AEpDropItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Particle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle"));

	Trigger->SetupAttachment(RootComponent);
	Trigger->SetHiddenInGame(true);

	Particle->SetupAttachment(Trigger);

	Mesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Mesh->SetCastShadow(false);
	Mesh->SetCollisionProfileName(TEXT("Trigger"));
	Mesh->AttachToComponent(Particle, FAttachmentTransformRules::KeepRelativeTransform, FName("NiagaraComponent"));
}

void AEpDropItemActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEpDropItemActor::BeginPlay()
{
	Super::BeginPlay();

	// test
	if (Particle)
	{
		Particle->SetColorParameter(TEXT("GunPad_Color"), EPColorPalette(GetWorld()).GetColor(EItemRarity::Common));
	}

	switch (GetNetMode())
	{
	case ENetMode::NM_ListenServer:
	case ENetMode::NM_Standalone:
	case ENetMode::NM_Client:
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AEpDropItemActor::OnBeginOverlap);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &AEpDropItemActor::OnEndOverlap);
	}
	break;

	default:
		break;
	}
}

void AEpDropItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Reset();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AEpDropItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}