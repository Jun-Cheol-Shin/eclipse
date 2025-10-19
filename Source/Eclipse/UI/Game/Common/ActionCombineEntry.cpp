// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCombineEntry.h"
#include "CommonActionWidget.h"
#include "CommonTextBlock.h"
#include "../../../Subsystems/EpInputManagerSubSystem.h"

#include "Engine/PlatformSettingsManager.h"
#include "CommonInputBaseTypes.h"


void UActionCombineEntry::SetKey(const FKey& Key)
{
	ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	if (!ensure(LocalPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Local player! %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	UEpInputManagerSubSystem* Subsystem = LocalPlayer->GetSubsystem<UEpInputManagerSubSystem>();
	if (!ensure(Subsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Subsystem! %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	FName GamepadName = Subsystem->GetGamepadName();
	ECommonInputType KeyInputType = Subsystem->GetInputType();
	FSlateBrush InputBrush;

	bool bSucess = UCommonInputPlatformSettings::Get()->TryGetInputBrush(InputBrush, TArray<FKey> { Key }, KeyInputType, GamepadName);

	if (ensure(bSucess) && nullptr != ActionWidget)
	{
		ActionWidget->SetIconRimBrush(InputBrush);
	}
}

void UActionCombineEntry::SetEnableSign(bool bEnable)
{
	if (nullptr != PlusText)
	{
		PlusText->SetVisibility(true == bEnable ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UActionCombineEntry::NativeConstruct()
{
	Super::NativeConstruct();

	SetEnableSign(false);
}

void UActionCombineEntry::RemoveFromParent()
{
	SetEnableSign(false);

	Super::RemoveFromParent();
}
