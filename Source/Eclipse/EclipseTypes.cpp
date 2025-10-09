// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseTypes.h"

FLinearColor UColorPaletteDataAsset::GetColor(EItemRarity InRarity) const
{
	if (Rarities.Contains(InRarity))
	{
		return Rarities[InRarity];
	}

	return FLinearColor::Transparent;
}
