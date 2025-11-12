// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "../../Common/EpTabListWidgetBase.h"
#include "MenuHub_Inventory.generated.h"

/**
 * 
 */
class UGridBasedListView;

UCLASS()
class ECLIPSE_API UMenuHub_Inventory : public UCommonUserWidget, public IEclipseTabContentInterface
{
	GENERATED_BODY()
	
	

protected:
	// IEclipseTabContentInterface
	virtual void NativeOnActiveContent() override;
	virtual void NativeOnInactiveContent() override;
	// End IEclipseTabContentInterface

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UGridBasedListView> InventoryList;

#if WITH_EDITOR
private:
	IConsoleCommand* InventoryCheatHandle = nullptr;
#endif
};
