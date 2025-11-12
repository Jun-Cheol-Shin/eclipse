// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "GridBasedListEntry.h"
#include "GridBasedListView.generated.h"

/**
 * 
 */
class UCanvasPanel;
class UBorder;
class USizeBox;
class UGridPanel;
class UCommonHierarchicalScrollBox;


UCLASS()
class ECLIPSE_API UGridBasedListView : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void AddItem(UGridBasedListItem* InItem);
	void RemoveItem(UGridBasedListItem* InItem);

	UUserWidget* GetEntry(UGridBasedListItem* InItem);
	UGridBasedListItem* GetItemFromListEntry(UUserWidget* InWidget);
	void RefreshList();	

	float GetSlotSize() const;
		 
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

private:
	void AddWidget(UGridBasedListItem* InItem);
	void RemoveWidget(UGridBasedListItem* InItem);

	// Get Top Left Index
	int32 MakeKey(uint32 InRow, uint32 InColumn);
	FVector2D IndexToLocalSize(uint32 InSlotW, uint32 InSlotH);
	int32 GetBlankedSpaceIndex(OUT TArray<int32>& OutGridList, uint8 InWidth, uint8 InHeight);

	void SetMaterial();
	void SetInventorySize();

	bool IsOverScroll(int32 InTopLeftKey) const;

private:
	UFUNCTION()
	void OnChangedScrollOffset(float InScrollOffset);

private:
	int32 CurrentPage = 0;
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

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TArray<FDoubleArrayIndexes> HiddenIndex{};
	


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
	TArray<UGridBasedListItem*> Grid;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UGridBasedListItem>, TWeakObjectPtr<UUserWidget>> ActiveWidgets;

	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<UUserWidget>, TObjectPtr<UGridBasedListItem>> ActiveItems;
};
