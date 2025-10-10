// Fill out your copyright notice in the Description page of Project Settings.


#include "EpDropItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"

#include "../PlayerCore/Component/Inventory/EclipseInventoryItem.h"
#include "../Subsystems/EpResourceSubSystem.h"

void AEpDropItemActor::Set(const UEclipseInventoryItem* InItem)
{
	// TODO : DropItemActor는 Pooling할 예정..

	ItemData = InItem;
	
	if (ItemData.IsValid())
	{
		EItemRarity RarityEnum = ItemData->GetRarity();
		TSoftObjectPtr<UStaticMesh> MeshPtr = ItemData->GetMesh();

		Particle->SetColorParameter(TEXT("User_GunPad_Color"), EPColorPalette(GetWorld()).GetColor(RarityEnum));
	}
}

// Sets default values
AEpDropItemActor::AEpDropItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Particle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle"));

	Trigger->SetupAttachment(RootComponent);
	Mesh->SetupAttachment(RootComponent);
	Mesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Mesh->SetCastShadow(false);
	Mesh->SetCollisionProfileName(TEXT("Trigger"));

	Particle->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, FName("NiagaraComponent"));

	Trigger->SetHiddenInGame(true);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AEpDropItemActor::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AEpDropItemActor::BeginPlay()
{
	Super::BeginPlay();
	
#if WITH_EDITOR
	FConsoleCommandWithArgsDelegate ConsoleDelegate;
	ConsoleDelegate.Unbind();
	ConsoleDelegate.BindUObject(this, &AEpDropItemActor::ShowDebugCollision);

	ConsoleDelHandle = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("showdebug.ingameactor"),
		TEXT("showdebug.ingameactor 0 = false, 1 = true"), ConsoleDelegate);

#endif 
}

void AEpDropItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_EDITOR
	IConsoleManager::Get().UnregisterConsoleObject(ConsoleDelHandle);
#endif

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AEpDropItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEpDropItemActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

#if WITH_EDITOR
void AEpDropItemActor::ShowDebugCollision(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
		return;

	int Flag = FCString::Atoi(*Args[0]);


	if (Trigger)
	{
		Trigger->SetHiddenInGame(0 == Flag ? false : true);
	}

}
#endif

