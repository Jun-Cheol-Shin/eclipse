// Fill out your copyright notice in the Description page of Project Settings.


#include "EpDropItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "NiagaraComponent.h"
#include "InputAction.h"

#include "../PlayerCore/EpPlayerController.h"
#include "../PlayerCore/Component/Inventory/EpInventoryComponent.h"
#include "../PlayerCore/Component/Inventory/EclipseInventoryItem.h"
#include "../Subsystems/EpResourceSubSystem.h"

#include "../Option/EpInputConfig.h"

#include "../PlayerCore/EpPlayerState.h"

#include "../GameModes/EpGameInstance.h"
#include "../Subsystems/EpUIManagerSubsystem.h"
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

void AEpDropItemActor::OnInteract()
{
	if (ensure(ItemData.IsValid() && OwningController.IsValid()))
	{	
		if (AEpPlayerState* PlayerState = OwningController->GetPlayerState<AEpPlayerState>())
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

void AEpDropItemActor::OnDirectUse()
{
	UE_LOG(LogTemp, Display, TEXT("Use Direct!"));
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
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AEpDropItemActor::NativeOnPreInteract);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &AEpDropItemActor::NativeOnEndInteract);
		SetContext(InputMapping.LoadSynchronous());
	}
	break;

	default:
		break;
	}
}

void AEpDropItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	IInteractable::Clear();
	Reset();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AEpDropItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// IInteractable
void AEpDropItemActor::BindAction(const UEpInputConfig* InConfig, UEnhancedInputComponent* InComponent, OUT TArray<TPair<uint32, TWeakObjectPtr<const UInputAction>>>& OutActions)
{
	check(InConfig);

	if (nullptr != InComponent)
	{
		const UInputAction* InteractAction = InConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag(FName("InputTag.Interact"), true));
		const UInputAction* PingAction = InConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag(FName("InputTag.Interact.Ping"), true));

		FEnhancedInputActionEventBinding& InteractHandle = InComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AEpDropItemActor::OnInteract);
		FEnhancedInputActionEventBinding& PingHandle = InComponent->BindAction(PingAction, ETriggerEvent::Started, this, &AEpDropItemActor::OnPing);

		OutActions.Emplace(InteractHandle.GetHandle(), InteractHandle.GetAction());
		OutActions.Emplace(PingHandle.GetHandle(), PingHandle.GetAction());

		// TODO : 바로 사용 가능한 아이템인지 체크 (바로 사용 액션)
		//int32 ItemId = ItemData->GetItemId();
		//const UInputAction* UseDirectAction = InConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag(FName("InputTag.Interact.UseDirect"), true));
		//InComponent->BindAction(UseDirec)

	}
}

void AEpDropItemActor::OnPreInteract_Implementation(AActor* OtherActor)
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
		TArray<FInteractActionParam> ParamList;

		for (int i = 0; i < Handles.Num(); ++i)
		{
			FInteractActionParam NewParam;

			if (!ensureAlways(Handles[i].Value.IsValid()))
			{
				continue;
			}

			NewParam.InputAction = Handles[i].Value.Get();
			NewParam.DisplayText = FText::FromString(TEXT("TEST"));

			ParamList.Emplace(NewParam);
		}

		UISubSystem->ShowLayerWidget<UInteractPrompt>(FOnCompleteLoadedWidgetSignature::CreateWeakLambda(this, [Params = MoveTemp(ParamList)](UCommonActivatableWidget* InPrompt)
			{
				if (UInteractPrompt* Prompt = Cast<UInteractPrompt>(InPrompt))
				{
					Prompt->Set(Params);
				}
			}));
	}


}

void AEpDropItemActor::OnEndInteract_Implementation(AActor* OtherActor)
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

// End IInteractable