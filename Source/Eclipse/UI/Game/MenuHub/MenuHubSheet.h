// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../GameLayout/EpActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "MenuHubSheet.generated.h"

/**
 * 
 */

class UEpTabListWidgetBase;

UCLASS()
class ECLIPSE_API UMenuHubSheet : public UEpActivatableWidget
{
	GENERATED_BODY()

public:
	void SelectTab(const FGameplayTag& InTabTag);

	
protected:

	// UEpActivatableWidget
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnBack() override;
	virtual void OnChangedInputDevice(ECommonInputType InInputType) override;
	// End UEpActivatableWidget

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const;
	
private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UEpTabListWidgetBase* TabList = nullptr;

};
