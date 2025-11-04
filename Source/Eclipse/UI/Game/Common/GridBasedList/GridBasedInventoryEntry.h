// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GridBasedInventoryEntry.generated.h"

/**
 * 
 */

class USizeBox;

UCLASS()
class ECLIPSE_API UGridBasedInventoryEntry : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Set(int32 InItemId);

private:

};
