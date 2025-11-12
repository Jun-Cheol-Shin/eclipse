// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "GridBasedFootprint.h"
#include "GridBasedObjectListEntry.h"
#include "GridBasedListEntry.generated.h"


class USizeBox;

UCLASS()
class UGridBasedListItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FDoubleArrayIndexes TopLeftPos{};

	UPROPERTY(EditAnywhere)
	FDoubleArrayIndexes TileSize{};

	UPROPERTY(EditAnywhere)
	TArray<FDoubleArrayIndexes> HiddenIndexes{};
};


UCLASS(Abstract, Blueprintable, MinimalAPI)
class UGridBasedListEntry : public UCommonUserWidget, public IGridBasedObjectListEntry
{
	GENERATED_BODY()

protected:
	// IGridBasedObjectListEntry
	virtual void NativeOnListItemObjectSet(UGridBasedListItem* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;
	// End IGridBasedObjectListEntry

	// UUserWidget
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
	// End UUserWidget

private:
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	UGridBasedFootprint* Footprint = nullptr;


private:
	UPROPERTY()
	TMap<FGameplayTag, FFootprintStyle> FootprintStyles;
};
