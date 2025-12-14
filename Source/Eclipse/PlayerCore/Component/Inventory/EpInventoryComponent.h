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
	GENERATED_BODY()
	
	FEclipseInventoryEntry() {}

	// Your data:
	UPROPERTY()
	TObjectPtr<UEclipseInventoryItem> ItemInstance = nullptr;

	// Optional: debug string used with LogNetFastTArray logging
	FString GetDebugString();

	// FFastArraySerializerItem
	//void PreReplicatedRemove(const struct FFastArraySerializer& InArraySerializer);
	//void PostReplicatedAdd(const struct FFastArraySerializer& InArraySerializer);
	//void PostReplicatedChange(const struct FFastArraySerializer& InArraySerializer);
	// End FFastArraySerializerItem
};

USTRUCT(BlueprintType)
struct FEclipseInventoryArray : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FEclipseInventoryArray() : OwnerComponent(nullptr) {}
	FEclipseInventoryArray(UActorComponent* InComponent) : OwnerComponent(InComponent) {};

public:
	void SetSize(const FIntPoint& InSize);


	TArray<UEclipseInventoryItem*> GetAllItems() const;

	// FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	// End FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FEclipseInventoryEntry, FEclipseInventoryArray>(ItemGrid, DeltaParms, *this);
	}

	UEclipseInventoryItem* AddItem(int32 InItemId, int32 InStackCount);
	void RemoveItem(UEclipseInventoryItem* InItem);
	bool IsEmpty(int Index) const;

private:
	int32 GetEmptyIndex(OUT TArray<int32>& OutIndexList, const FIntPoint& InItemSize) const;
	FIntPoint IndexToPoint(int32 Index) const;
	int32 PointToIndex(const FIntPoint& InPoint) const;
	bool IsEmptySpace(const FIntPoint& InTopLeftPoint, const FIntPoint& InSize) const;
	bool GetIndex(OUT TArray<int32>& OutIndexList, const FIntPoint& InTopLeft, const FIntPoint& InSize) const;

private:
	UPROPERTY()
	TArray<FEclipseInventoryEntry> ItemGrid;	/** Step 3: You MUST have a TArray named Items of the struct you made in step 1. */

	TSet<UEclipseInventoryItem*> Items;

private:
	TObjectPtr<UActorComponent> OwnerComponent = nullptr;

private:
	UPROPERTY(NotReplicated)
	FIntPoint InventorySize = FIntPoint(0, 0);

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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddedItemSignature, UEclipseInventoryItem*, InChangedItem);
	FOnAddedItemSignature OnAddedItemDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovedItemSignature, int32, InTopLeft);
	FOnRemovedItemSignature OnRemovedItemDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFailedItemSignature, const FString&, InFailedMsg);
	FOnFailedItemSignature OnFailedMessageDelegate;

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItem(int32 InItemId, int32 InStackCount);

	UFUNCTION(Server, Reliable)
	void Server_RemoveItem(UEclipseInventoryItem* InRemovedItem);

	
public:
	// TODO : 서버에서 호출이 된다는 전제 하에 호출 해야함. ex ) Lyragame -> GameAbility_Collect
	//UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	//void AddItem(int32 InItemId);


public:	
	// Sets default values for this component's properties
	UEpInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void InitializeComponent() override;

	// Replicated
	virtual bool ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags) override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE const FIntPoint& GetInventorySize() const { return InventorySize; }

protected:
	UPROPERTY(Replicated)
	FEclipseInventoryArray InventoryItemArray;

	UPROPERTY(EditAnywhere)
	FIntPoint InventorySize;
};
 