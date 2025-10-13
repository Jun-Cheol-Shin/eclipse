// Fill out your copyright notice in the Description page of Project Settings.


#include "EpDropItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "../PlayerCore/EpPlayerController.h"

#include "../PlayerCore/Component/Inventory/EclipseInventoryItem.h"
#include "../Subsystems/EpResourceSubSystem.h"

#include "InputAction.h"
#include "../Option/EpInputConfig.h"


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

void AEpDropItemActor::OnInteract()
{
	UE_LOG(LogTemp, Display, TEXT("Interact!"));
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

	SetContext(InputMapping.LoadSynchronous());
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
void AEpDropItemActor::BindAction(const UEpInputConfig* InConfig, UEnhancedInputComponent* InComponent, OUT TMap<uint32, TWeakObjectPtr<const UInputAction>>& OutActions)
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
	UE_LOG(LogTemp, Warning, TEXT("On Pre Interact c++"));
}

void AEpDropItemActor::OnEndInteract_Implementation(AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("On End Interact c++"));
}

// End IInteractable