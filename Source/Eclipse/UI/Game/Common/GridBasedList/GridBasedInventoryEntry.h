// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GridBasedInventoryEntry.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FDoubleArrayIndexes
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int X = 0;

	UPROPERTY(EditAnywhere)
	int Y = 0;
};

class UMaterialInstanceDynamic;
class UImage;
class USizeBox;

UCLASS(Abstract, Blueprintable, MinimalAPI)
class UGridBasedInventoryEntry : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void Set(int32 InRow, int32 InColumn, float InSlotSize, const TArray<int32>& InHidden);

protected:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

private:
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* GridWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", Category = "Inventory Setting"))
	TObjectPtr<UTexture> SlotTexture = nullptr;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* CachedInstance = nullptr;


#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int Row = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	int Column = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float SlotSize = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FDoubleArrayIndexes> ExcludeHiddenIdx;

#endif

};
