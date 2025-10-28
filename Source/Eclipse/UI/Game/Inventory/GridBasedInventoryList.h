// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/UserWidgetPool.h"
#include "GridBasedInventoryList.generated.h"

/**
 * 
 */


UCLASS()
class ECLIPSE_API UGridBasedInventoryListItem : public UObject
{
	GENERATED_BODY()

public:


};

class UCanvasPanel;
class UBorder;
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

	virtual void SynchronizeProperties() override;

private:

	void SetMaterial();
	void SetInventorySize();

private:
	UFUNCTION()
	void OnChangedScrollOffset(float InScrollOffset);

private:
	FUserWidgetPool SlotPool {};
	int32 CurrentScrollOffset = 0;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 SlotSize = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 RowCount = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	int32 ColumnCount = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TObjectPtr<UTexture> SlotTexture = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TSubclassOf<UGridBasedInventoryEntry> ItemWidgetClass = nullptr;
	
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
	TArray<UGridBasedInventoryListItem*> ListItems;
};
