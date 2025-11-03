// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "../../Common/EpTabListWidgetBase.h"
#include "MenuHub_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UMenuHub_Inventory : public UCommonUserWidget, public IEclipseTabContentInterface
{
	GENERATED_BODY()
	
	
public:


protected:
	// IEclipseTabContentInterface
	virtual void NativeOnCreatedContentWidget() override;
	virtual void NativeOnActiveContent() override;
	virtual void NativeOnInactiveContent() override;
	// End IEclipseTabContentInterface
};
