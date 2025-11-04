// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "GridBasedInventoryList.generated.h"

/**
 * 
 */
class UCanvasPanel;
class UBorder;
class USizeBox;
class UGridPanel;
class UCommonHierarchicalScrollBox;

UCLASS()
class UGridBasedInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	// Top Left Info
	FVector2D TopLeft = FVector2D();

	// Size
	FVector2D Size = FVector2D();
};


UCLASS()
class ECLIPSE_API UGridBasedInventoryList : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void AddItem(UGridBasedInventoryItem* InItem);
	void RemoveItem(UGridBasedInventoryItem* InItem);

	UUserWidget* GetEntry(UGridBasedInventoryItem* InItem);
	UGridBasedInventoryItem* GetItemFromListEntry(UUserWidget* InWidget);
	void RefreshList();	

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

private:
	void AddWidget(UGridBasedInventoryItem* InItem);

	// Get Top Left Index
	int32 MakeKey(uint32 InRow, uint32 InColumn);
	int32 GetBlankedSpaceIndex(uint32 InSlotW, uint32 InSlotH);
	FVector2D ConvertCanvasPosition(uint32 InSlotW, uint32 InSlotH);

	void SetMaterial();
	void SetInventorySize();

	bool IsOverScroll(int32 InTopLeftKey) const;

private:
	UFUNCTION()
	void OnChangedScrollOffset(float InScrollOffset);

private:
	FUserWidgetPool ItemPool {};

private:
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 SlotSize = 0;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 RowCount = 0;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 ColumnCount = 0;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TObjectPtr<UTexture> SlotTexture = nullptr;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting", MustImplement = "GridBasedObjectListEntry"))
	TSubclassOf<UUserWidget> ItemWidgetClass = nullptr;
	
private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USizeBox> InventorySizeBox = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonHierarchicalScrollBox> ScrollBox = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UBorder> InventoryBG = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCanvasPanel> InventoryPanel = nullptr;



private:
	UPROPERTY(Transient)
	TArray<UGridBasedInventoryItem*> Grid;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UGridBasedInventoryItem>, TWeakObjectPtr<UUserWidget>> ActiveWidgets;

};
