// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EpInventoryComponent.generated.h"

class UEclipseInventoryItem;

USTRUCT(BlueprintType)
struct FEclipseInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
	
	FEclipseInventoryEntry() {}

	// Your data:
	UPROPERTY()
	TObjectPtr<UEclipseInventoryItem> ItemInstance = nullptr;

	/**
	 * Optional functions you can implement for client side notification of changes to items;
	 * Parameter type can match the type passed as the 2nd template parameter in associated call to FastArrayDeltaSerialize
	 *
	 * NOTE: It is not safe to modify the contents of the array serializer within these functions, nor to rely on the contents of the array
	 * being entirely up-to-date as these functions are called on items individually as they are updated, and so may be called in the middle of a mass update.
	 */
	//void PreReplicatedRemove(const struct FExampleArray& InArraySerializer);
	//void PostReplicatedAdd(const struct FExampleArray& InArraySerializer);
	//void PostReplicatedChange(const struct FExampleArray& InArraySerializer);

	// Optional: debug string used with LogNetFastTArray logging
	//FString GetDebugString();

};

USTRUCT(BlueprintType)
struct FEclipseInventoryArray : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

public:
	FEclipseInventoryArray() {}
	FEclipseInventoryArray(UActorComponent* InOwnerComponent) : CachedOwnerComponent(InOwnerComponent) {}

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

	void AddItem(UEclipseInventoryItem* InItem);
	void AddItem(UEpInventoryComponent* InComponent);
	void RemoveItem(UEclipseInventoryItem* InItem);


private:
	UPROPERTY()
	TArray<FEclipseInventoryEntry>	Items;	/** Step 3: You MUST have a TArray named Items of the struct you made in step 1. */
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedItemSignature, UEclipseInventoryItem*, InChangedItem);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECLIPSE_API UEpInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEpInventoryComponent();
		
public:
	FOnChangedItemSignature OnAddedItemDelegate;
	FOnChangedItemSignature OnRemovedItemDelegate;
	
};
 