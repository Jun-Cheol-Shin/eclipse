// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridBasedFootprint.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDoubleArrayIndexes
{
	GENERATED_BODY()

public:
	FDoubleArrayIndexes() : X(0), Y(0) {}
	FDoubleArrayIndexes(int x, int y) : X(x), Y(y) {}

	UPROPERTY(EditAnywhere)
	int X = 0;

	UPROPERTY(EditAnywhere)
	int Y = 0;
};

class UImage;
class UTexture;


USTRUCT(BlueprintType)
struct FFootprintStyle
{
	GENERATED_BODY()

public:
	FFootprintStyle() {};
	FFootprintStyle(UTexture* InTexture, const FSlateColor& InColor) : FootprintTexture(InTexture), FootprintColor(InColor) {}

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture> FootprintTexture = nullptr;

	UPROPERTY(EditAnywhere)
	FSlateColor FootprintColor = FLinearColor::White;
};


UCLASS()
class ECLIPSE_API UGridBasedFootprint : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetStyle(const FFootprintStyle& InStyle);
	void SetFootprint(int InRow, int InColumn, const TArray<int>& InHiddenIndex);
	
protected:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
	
private:
	UPROPERTY()
	UMaterialInstanceDynamic* CachedInstance = nullptr;
	
#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", Category = "Preview"))
	int Row = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", Category = "Preview"))
	int Column = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", Category = "Preview"))
	TArray<FDoubleArrayIndexes> ExcludeHiddenIdx;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", Category = "Preview"))
	FFootprintStyle PreviewStyle;
#endif

private:
	UPROPERTY(meta = (BindWidget))
	UImage* FootprintImage = nullptr;
	
};
