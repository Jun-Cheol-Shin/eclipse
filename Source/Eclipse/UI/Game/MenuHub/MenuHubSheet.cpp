// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuHubSheet.h"
#include "../Common/EpTabListWidgetBase.h"

#include "Eclipse/SubSystems/EpUIManagerSubsystem.h"

void UMenuHubSheet::SelectTab(const FGameplayTag& InTabTag)
{
	if (nullptr != TabList)
	{
		TabList->SelectTab(InTabTag);
	}
}

void UMenuHubSheet::OnShow()
{

}

void UMenuHubSheet::OnHide()
{

}

void UMenuHubSheet::OnCreate()
{

}

void UMenuHubSheet::OnDestroy()
{

}

void UMenuHubSheet::OnBack()
{
	UEpUIManagerSubsystem* UIManager = UGameInstance::GetSubsystem<UEpUIManagerSubsystem>(GetGameInstance());
	if (!ensure(UIManager))
	{
		return;
	}


	UIManager->HideLayerWidget<UMenuHubSheet>();
}


void UMenuHubSheet::OnChangedInputDevice(ECommonInputType InInputType)
{

}

TOptional<FUIInputConfig> UMenuHubSheet::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::CaptureDuringMouseDown, EMouseLockMode::DoNotLock);
}
