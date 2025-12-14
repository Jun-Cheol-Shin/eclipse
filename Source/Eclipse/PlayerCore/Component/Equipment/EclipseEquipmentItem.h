// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EclipseEquipmentItem.generated.h"

UCLASS()
class ECLIPSE_API UEclipseEquipmentItem : public UObject
{
	GENERATED_BODY()
	
public:
	void SetEquipItem(int32 InItemId);

	virtual void SpawnEquipmentActors(int32 InItemId);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:
	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	//~End of UObject interface

	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;


private:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
	
};
