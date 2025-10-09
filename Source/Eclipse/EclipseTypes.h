// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EclipseTypes.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common = 0,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Unique,
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EClassType : uint8
{
	None = 0		UMETA(Hidden),
	Knight = 1 << 1,
	Assassin = 1 << 2,
	Berserker = 1 << 3
};


UCLASS()
class ECLIPSE_API UColorPaletteDataAsset : public UDataAsset 
{
	GENERATED_BODY()

public:
	FLinearColor GetColor(EItemRarity InRarity) const;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<EItemRarity, FLinearColor> Rarities;

};