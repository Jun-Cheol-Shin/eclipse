// Fill out your copyright notice in the Description page of Project Settings.


#include "GridBasedInventoryEntry.h"

#include "Components/SizeBox.h"



void UGridBasedInventoryEntry::SetSize(const FVector2D& InSize)
{
	if (nullptr != SlotSizeBox)
	{
		SlotSizeBox->SetHeightOverride(InSize.Y);
		SlotSizeBox->SetWidthOverride(InSize.X);
	}
}