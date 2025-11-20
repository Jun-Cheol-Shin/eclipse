// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "GridBasedListEntry.h"
#include "DragDetectable.h"
#include "GridBasedListView.generated.h"

/**
 * 
 */
class UCanvasPanel;
class UBorder;
class USizeBox;
class UGridPanel;
class UCommonHierarchicalScrollBox;
class UGridBasedFootprint;

UENUM(BlueprintType)
enum class EStorageState : uint8
{
	Empty = 0,
	IsFull
};


UCLASS()
class ECLIPSE_API UGridBasedListView : public UCommonUserWidget, public IDragDetectable
{
	GENERATED_BODY()
	
public:
	void AddItem(UGridBasedListItem* InItem);
	void RemoveItem(UGridBasedListItem* InItem);

	const UUserWidget* GetEntry(UGridBasedListItem* InItem) const;
	const UGridBasedListItem* GetItemFromListEntry(UUserWidget* InWidget) const;
	void RefreshList();	

	float GetSlotSize() const;
		 
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

	// IDragDetectable
	virtual void NativeOnStartDetectDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent) override;
	virtual void NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent) override;
	virtual void NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent) override;
	virtual void NativeOnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent) override;
	// End IDragDetectable

private:
	void AddWidget(UGridBasedListItem* InItem);
	void RemoveWidget(UGridBasedListItem* InItem);

	// Get Top Left Index
	int32 MakeKey(uint32 InRow, uint32 InColumn);
	FVector2D GetLocalFromIndex(uint32 InSlotW, uint32 InSlotH);
	bool GetIndexFromLocal(const FVector2D InPos, OUT uint32& OutWIdx, uint32& OutHIdx);
	int32 GetBlankedSpaceIndex(OUT TArray<int32>& OutGridList, uint8 InWidth, uint8 InHeight);

	void SetMaterial();
	void SetInventorySize();

	bool IsOverScroll(int32 InTopLeftKey) const;
	bool IsEmptySpace(int32 InTopLeftKey, const UGridBasedListItem* InItem) const;

private:
	UFUNCTION()
	void OnChangedScrollOffset(float InScrollOffset);

private:
	int32 CurrentPage = 0;
	FUserWidgetPool ItemPool {};

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 SlotSize = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 RowCount = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 ColumnCount = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TObjectPtr<UTexture> SlotTexture = nullptr;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting", MustImplement = "GridBasedObjectListEntry"))
	TSubclassOf<UUserWidget> ItemWidgetClass = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
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

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UGridBasedFootprint> FootprintWidget = nullptr;



private:
	UPROPERTY(Transient)
	TArray<UGridBasedListItem*> Grid;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UGridBasedListItem>, UUserWidget*> ActiveWidgets;

	UPROPERTY(Transient)
	TMap<UUserWidget*, TObjectPtr<UGridBasedListItem>> ActiveItems;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EStorageState, FFootprintStyle> FootprintStyles;
};
