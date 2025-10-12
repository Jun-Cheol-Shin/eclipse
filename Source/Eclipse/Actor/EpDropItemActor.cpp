// Fill out your copyright notice in the Description page of Project Settings.


#include "EpDropItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "../PlayerCore/EpPlayerController.h"

#include "../PlayerCore/Component/Inventory/EclipseInventoryItem.h"
#include "../Subsystems/EpResourceSubSystem.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "../Option/EpInputConfig.h"

void AEpDropItemActor::NativeOnInteract()
{
	UE_LOG(LogTemp, Error, TEXT("Begin Interact!!!!!!!!!"));
}


void AEpDropItemActor::Set(const UEclipseInventoryItem* InItem)
{
	// TODO : DropItemActor는 Pooling할 예정..

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

// Called when the game starts or when spawned
void AEpDropItemActor::BeginPlay()
{
	Super::BeginPlay();

	// test
	if (Particle)
	{
		Particle->SetColorParameter(TEXT("GunPad_Color"), EPColorPalette(GetWorld()).GetColor(EItemRarity::Common));
	}


	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AEpDropItemActor::NativeOnPreInteract);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AEpDropItemActor::NativeOnEndInteract);
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


// IInteractable
const UInputAction* AEpDropItemActor::GetAction(APlayerController* InOwningController) const
{
	AEpPlayerController* PlayerController = Cast<AEpPlayerController>(InOwningController);
	if (nullptr == PlayerController)
	{
		return nullptr;
	}

	UEpInputConfig* Config = PlayerController->GetInputConfig();
	if (!ensure(Config))
	{
		return nullptr;
	}

	if (const UInputAction* InteractAction = Config->FindNativeInputActionForTag(
		FGameplayTag::RequestGameplayTag(FName("InputTag.Interact"), true)))
	{
		return InteractAction;
	}

	return nullptr;
}

int32 AEpDropItemActor::BindInteract(const UInputAction* InAction, UEnhancedInputComponent* InComponent)
{
	if (InComponent)
	{
		auto& Handle = InComponent->BindAction(InAction, ETriggerEvent::Started, this, &AEpDropItemActor::NativeOnInteract);
		return Handle.GetHandle();
	}

	return INDEX_NONE;
}

void AEpDropItemActor::OnPreInteract_Implementation(AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("On Pre Interact c++"));
}

void AEpDropItemActor::OnEndInteract_Implementation(AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("On End Interact c++"));
}

// End IInteractable