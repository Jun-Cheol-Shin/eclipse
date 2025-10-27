// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "GridBasedInventoryList.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FGridCoordinate
{
	GENERATED_BODY()

public:
	int32 Row = 0;
	int32 Column = 0;

	FVector2D Size = FVector2D::ZeroVector;
};


UCLASS()
class ECLIPSE_API UGridBasedInventoryListItem : public UObject
{
	GENERATED_BODY()

public:


};


class USizeBox;
class UGridPanel;
class UCommonHierarchicalScrollBox;

UCLASS()
class ECLIPSE_API UGridBasedInventoryList : public UCommonUserWidget
{
	GENERATED_BODY()
	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnChangedScrollOffset(float InScrollOffset);

	UFUNCTION(CallInEditor)
	void DebugInventoryList();

private:
	FUserWidgetPool SlotPool {};
	int32 CurrentScrollOffset = 0;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int32 SlotHeight = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int32 SlotWidth = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int32 SlotRow = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int32 SlotColumn = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGridBasedInventoryEntry> EntryClass = nullptr;
	
private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USizeBox> InventorySizeBox = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UGridPanel> GridPanel = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonHierarchicalScrollBox> ScrollBox = nullptr;


private:
	UPROPERTY(Transient)
	TMap<FGridCoordinate, UGridBasedInventoryListItem*> ListItems;
};
