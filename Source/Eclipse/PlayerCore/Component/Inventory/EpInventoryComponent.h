// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EpInventoryComponent.generated.h"

class UEclipseInventoryItem;
class UActorComponent;
class AEpDropItemActor;

USTRUCT(BlueprintType)
struct FEclipseInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
	
	FEclipseInventoryEntry() {}

	// Your data:
	UPROPERTY()
	TObjectPtr<UEclipseInventoryItem> ItemInstance = nullptr;

	// Optional: debug string used with LogNetFastTArray logging
	FString GetDebugString();
};

USTRUCT(BlueprintType)
struct FEclipseInventoryArray : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

public:
	FEclipseInventoryArray() { InventorySize = FIntPoint(0, 0); }
	FEclipseInventoryArray(UActorComponent* InOwnerComponent, const FIntPoint& InInventorySize) : CachedOwnerComponent(InOwnerComponent), InventorySize(InInventorySize) 
	{
		Items.Reset();
		Items.SetNumZeroed(InInventorySize.X * InInventorySize.Y);
	}

	TArray<UEclipseInventoryItem*> GetAllItems() const;

	// FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	// End FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FEclipseInventoryEntry, FEclipseInventoryArray>(Items, DeltaParms, *this);
	}

	void AddItem(UEpInventoryComponent* InComponent);
	void AddItem(UEclipseInventoryItem* InItem);
	void RemoveItem(UEclipseInventoryItem* InItem);

	bool IsEmpty(int Index) const;

private:
	UPROPERTY()
	TArray<FEclipseInventoryEntry>	Items;	/** Step 3: You MUST have a TArray named Items of the struct you made in step 1. */

	FIntPoint InventorySize;
	TWeakObjectPtr<UActorComponent> CachedOwnerComponent;
};

template<>
struct TStructOpsTypeTraits< FEclipseInventoryArray > : public TStructOpsTypeTraitsBase2< FEclipseInventoryArray >
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECLIPSE_API UEpInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItem(UEclipseInventoryItem* InAddItem);

public:
	const FIntPoint& GetInventorySize() const;
	int GetIndex(const FIntPoint& InPoint) const;

	bool IsPossibleAdd(UEclipseInventoryItem* InItem) const;
	//bool IsPossibleAdd(UEclipseInventoryItem* InItem, const FIntPoint& InTopLeft) const;

public:	
	// Sets default values for this component's properties
	UEpInventoryComponent();
		
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedItemSignature, UEclipseInventoryItem*, InChangedItem);
	FOnChangedItemSignature OnAddedItemDelegate;
	FOnChangedItemSignature OnRemovedItemDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFailedItemSignature, const FString&, InFailedMsg);
	FOnFailedItemSignature OnFailedMessageDelegate;

public:

	void RemoveItem(UEclipseInventoryItem* InRemovedItem);

protected:
	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InTickFunc) override;

protected:
	UPROPERTY(Replicated)
	FEclipseInventoryArray InventoryItemArray;


	UPROPERTY(EditAnywhere)
	FIntPoint InventorySize = FIntPoint(0, 0);
};
 