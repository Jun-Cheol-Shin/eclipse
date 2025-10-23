// Fill out your copyright notice in the Description page of Project Settings.


#include "EpActivatableWidget.h"

#include "EnhancedInput/Public/InputMappingContext.h"
#include "EnhancedInput/Public/InputAction.h"

#include "../../Subsystems/EpInputManagerSubSystem.h"
#include "../../Subsystems/EpUIManagerSubsystem.h"
#include "../../GameModes/EpGameInstance.h"

#include "EnhancedInputComponent.h"

void UEpActivatableWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    if (false == InputActions.IsEmpty())
    {
        APlayerController* PlayerController = GetOwningPlayer();
        if (ensureAlways(PlayerController))
        {
            InputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
        }
    }

    OnShow();

    ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
    if (ensureAlways(LocalPlayer))
    {
        if (UEpInputManagerSubSystem* UISubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubSystem>())
        {
            UISubSystem->OnInputMethodChangedNative.AddUObject(this, &UEpActivatableWidget::OnChangedInputDevice);
            OnChangedInputDevice(UISubSystem->GetInputType());
        }        
    } 
}

void UEpActivatableWidget::NativeOnDeactivated()
{
    ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
    if (ensureAlways(LocalPlayer))
    {
        if (UEpInputManagerSubSystem* UISubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubSystem>())
        {
            UISubSystem->OnInputMethodChangedNative.RemoveAll(this);
        }
    }

    if (InputComponent.IsValid())
    {
        InputComponent->ClearActionBindings();
    }

    InputComponent.Reset();

    OnHide();

    Super::NativeOnDeactivated();
}

void UEpActivatableWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();


	if (nullptr != InputMapping)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = InputMapping->GetMappings();

        for (const FEnhancedActionKeyMapping& Map : Mappings)
        {
            if (Map.Action)
            {
                InputActions.Add(Map.Action);
            }
        }

	}
}
