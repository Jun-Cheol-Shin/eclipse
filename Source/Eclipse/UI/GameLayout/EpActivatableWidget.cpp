// Fill out your copyright notice in the Description page of Project Settings.


#include "EpActivatableWidget.h"

#include "EnhancedInput/Public/InputMappingContext.h"
#include "EnhancedInput/Public/InputAction.h"

#include "../../Subsystems/EpInputManagerSubSystem.h"
#include "../../Subsystems/EpUIManagerSubsystem.h"
#include "../../GameModes/EpGameInstance.h"



#include "Input/CommonUIInputTypes.h"


void UEpActivatableWidget::BindUIAction(const FName& InActionAssetName, ETriggerEvent TriggerEvent, UObject* InObject, FName InFunctionName)
{
    if (ensure(InputComponent.IsValid()) && InputActions.Contains(InActionAssetName))
    {
        const UInputAction* Action = InputActions[InActionAssetName];

        if (!ensureAlways(Action)) return;

        FEnhancedInputActionEventBinding& ActionEventBinding = InputComponent->BindAction(InputActions[InActionAssetName], TriggerEvent, InObject, InFunctionName);
        ActionHandles.Emplace(&ActionEventBinding);
    }
}

void UEpActivatableWidget::RemoveUIAction(const FName& InActionAssetName)
{
    for (auto& Handle : ActionHandles)
    {
        if (!ensureAlways(Handle && Handle->GetAction())) continue;

        if (InActionAssetName.IsEqual(Handle->GetAction()->GetFName()))
        {
            InputComponent->RemoveBindingByHandle(Handle->GetHandle());
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Not Bounded Action %s"), ANSI_TO_TCHAR(__FUNCTION__));
}

const UInputAction* UEpActivatableWidget::GetAction(const FName& InActionAssetName)
{
    if (false == InputActions.Contains(InActionAssetName))
    {
        return nullptr;
    }

    return InputActions[InActionAssetName];
}

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
        if (UEpInputManagerSubsystem* UISubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubsystem>())
        {
            UISubSystem->OnInputMethodChangedNative.AddUObject(this, &UEpActivatableWidget::OnChangedInputDevice);
            OnChangedInputDevice(UISubSystem->GetInputType());
        }        
    }
}

void UEpActivatableWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    OnHide();

    ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
    if (LocalPlayer)
    {
        if (UEpInputManagerSubsystem* UISubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubsystem>())
        {
            UISubSystem->OnInputMethodChangedNative.RemoveAll(this);
        }
    }

    if (InputComponent.IsValid())
    {
        for (auto& Handle : ActionHandles)
        {
            if (!ensure(Handle)) continue;
            InputComponent->RemoveBindingByHandle(Handle->GetHandle());
        }
    }

    ActionHandles.Reset();
    InputComponent.Reset();
}

void UEpActivatableWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();


}

void UEpActivatableWidget::NativeConstruct()
{
    Super::NativeConstruct();

    OnCreate();
}

void UEpActivatableWidget::NativeDestruct()
{
    OnDestroy();

    ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
    if (LocalPlayer)
    {
        if (UEpInputManagerSubsystem* UISubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubsystem>())
        {
            UISubSystem->OnInputMethodChangedNative.RemoveAll(this);
        }
    }

    if (InputComponent.IsValid())
    {
        for (auto& Handle : ActionHandles)
        {
            if (!ensure(Handle)) continue;
            InputComponent->RemoveActionBindingForHandle(Handle->GetHandle());
        }
    }

    ActionHandles.Reset();
    InputComponent.Reset();

    Super::NativeDestruct();
}

void UEpActivatableWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

    InputActions.Reset();

	if (nullptr != InputMapping)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = InputMapping->GetMappings();

        for (const FEnhancedActionKeyMapping& Map : Mappings)
        {
            if (nullptr != Map.Action && false == InputActions.Contains(Map.Action->GetFName()))
            {
                InputActions.Add(Map.Action->GetFName(), Map.Action);
            }
        }

	}
}

bool UEpActivatableWidget::NativeOnHandleBackAction()
{
    if (bIsBackHandler)
    {
        // boolean 변수를 쓰질 않는다?
        BP_OnHandleBackAction();
        OnBack();

        return true;
    }


    return false;
}
