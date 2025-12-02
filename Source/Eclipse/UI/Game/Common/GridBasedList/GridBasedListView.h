// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "GridBasedListEntry.h"
#include "DragDetectable.h"
#include "Draggable.h"
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
class ECLIPSE_API UGridBasedListView : public UCommonUserWidget, public IDragDetectable, public IDraggable
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGeneratedEntry, UGridBasedListEntry&)
	FOnGeneratedEntry OnEntryGenerated;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnReleasedEntry, UGridBasedListEntry&)
	FOnGeneratedEntry OnEntryReleased;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSelectionChanged, UGridBasedListItem*, bool)
	FOnSelectionChanged OnSelectionChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDropItemSignature, UGridBasedListItem*)
	FOnDropItemSignature OnDropItem;

public:
	void SetListItems(const TArray<UGridBasedListItem*>& InItemList);
	void ClearListItems();
	void AddItem(UGridBasedListItem* InItem);
	void RemoveItem(UGridBasedListItem* InItem);

	const UUserWidget* GetEntry(UGridBasedListItem* InItem) const;
	const UGridBasedListItem* GetItemFromListEntry(UUserWidget* InWidget) const;
	void RefreshList();	

	void SetSelect(const UGridBasedListItem* InItem);
	bool IsSelectedItem(const UGridBasedListItem* InItem) const;

	float GetSlotSize() const;
	int GetColumnCount() const { return ColumnCount; }
	int GetRowCount() const { return RowCount; }

	void SetVisibleScrollBar(bool bIsVisible);

	void Refresh();
		 
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void SynchronizeProperties() override;

	// Drag Detectable Event
	virtual void NativeOnStartDetectDrag(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);
	virtual void NativeOnDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);
	virtual void NativeOnEndDetect(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);
	virtual void NativeOnDrop(UUserWidget* InDraggingWidget, const FPointerEvent& InEvent);
	// End Drag Detectable Event

	// Draggable Event
	virtual void NativeOnDragCancel(UPanelSlot* InSlot) override;
	// End Draggable Event

	// UUserWidget Event
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;	
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	// End UUserWidget Event


private:
	// Make Top Left Index
	int32 MakeKey(int32 InRow, int32 InColumn);
	FVector2D PointToLocal(const FIntPoint& InPoint);
	FIntPoint LocalToPoint(const FVector2D& InLocalVec);
	FIntPoint KeyToPoint(int32 InKey);

	int32 GetEmptyTopLeftKey(OUT TArray<int32>& OutGridList, const FIntPoint& InItemSize);
	bool IsEmptySpace(const FIntPoint& InTopLeftPoint, const FIntPoint& InSize, const UGridBasedListItem* InExceptItem = nullptr);
	bool GetIndexes(OUT TArray<int32>& OutIndexList, const FIntPoint& InTopLeft, const FIntPoint& InSize);


private:
	void AddWidget(UGridBasedListItem* InItem);
	void RemoveWidget(const UGridBasedListItem* InItem);

	void SetMaterial();
	void SetInventorySize();

	bool IsOverScroll(int32 InTopLeftKey) const;
	void ForEach(int32 InX, int32 InY, const FIntPoint& InSize, TFunctionRef<void(int32 /* Grid Index */)> InFunc);

	void Swap(UGridBasedListEntry* InFirst, UGridBasedListEntry* InSecond);
	void SetItemPosition(UGridBasedListItem* InItem, const FIntPoint& InPosition);

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

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	FLinearColor SlotColor = FLinearColor::White;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting", MustImplement = "/Script/Eclipse.GridBasedObjectListEntry"))
	TSubclassOf<UUserWidget> ItemWidgetClass = nullptr;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	EDragPivot DragPivot;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	FVector2D DragOffset;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TArray<FIntPoint> HiddenIndex{};
	


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
	TArray<const UGridBasedListItem*> Grid;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UGridBasedListItem>, UUserWidget*> ActiveWidgets;

	UPROPERTY(Transient)
	TMap<UUserWidget*, TObjectPtr<UGridBasedListItem>> ActiveItems;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EStorageState, FFootprintStyle> FootprintStyles;
	
	UPROPERTY(Transient)
	TArray<UGridBasedListItem*> ListItems;

	UPROPERTY(Transient)
	TSet<const UGridBasedListItem*> SelectedItems;

	UPROPERTY(Transient)
	TSet<const UGridBasedListItem*> DeActivedSelectedItems;

private:
	FIntPoint TempFootprintLoc = FIntPoint(-1, -1);
};
