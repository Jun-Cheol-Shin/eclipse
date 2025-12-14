// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseEquipmentItem.h"
#include "Net/UnrealNetwork.h"
//#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"

#include "GameFramework/Character.h"
#include "../../../DataTable/EquipmentDataRow.h"
#include "../../../EclipseFunctionLibrary.h"
#include "../../../Subsystems/EpGameDataSubSystem.h"

void UEclipseEquipmentItem::SetEquipItem(int32 InItemId)
{
	DestroyEquipmentActors();
	SpawnEquipmentActors(InItemId);

	// 장비 별 어빌리티 세팅?

}

void UEclipseEquipmentItem::SpawnEquipmentActors(int32 InItemId)
{
	if (APawn* OwningPawn = Cast<APawn>(GetOuter()))
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}


		UEpGameDataSubsystem* GameDataManager = UEclipseFunctionLibrary::GetGameDataSubSytem(GetWorld());
		if (!ensureAlways(GameDataManager))
		{
			return;
		}

		TArray<const FEquipmentDataRow*> EquipDataList;
		if (true == GameDataManager->GetGameDataList<FEquipmentDataRow>(InItemId, OUT EquipDataList))
		{
			for (const FEquipmentDataRow* Row : EquipDataList)
			{
				FSoftObjectPath ActorPath = Row->ActorToSpawn;
				TSoftClassPtr<AActor> SoftClass(ActorPath);

				UClass* ActorClass = SoftClass.LoadSynchronous();
				if (nullptr == ActorClass) { continue; }

				AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity, OwningPawn);
				NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
				NewActor->SetActorRelativeTransform(Row->AttachTransform);
				NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, Row->AttachSocket);

				SpawnedActors.Add(NewActor);

			}
		}
	}
}

void UEclipseEquipmentItem::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void UEclipseEquipmentItem::OnEquipped()
{

}

void UEclipseEquipmentItem::OnUnequipped()
{

}

UWorld* UEclipseEquipmentItem::GetWorld() const
{
	if (APawn* OwningPawn = Cast<APawn>(GetOuter()))
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

void UEclipseEquipmentItem::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	Super::RegisterReplicationFragments(Context, RegistrationFlags);

	// Build descriptors and allocate PropertyReplicationFragments for this object
	//FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

void UEclipseEquipmentItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SpawnedActors);
}