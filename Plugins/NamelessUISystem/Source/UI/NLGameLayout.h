// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "NLGameLayout.generated.h"


struct FStreamableHandle;
class UCommonActivatableWidgetContainerBase;
class UCommonActivatableWidget;
class UNLGameLayer;


/* DEPRECATED
UENUM(BlueprintType)
enum class EEclipseGameLayer : uint8
{
	System = 0,			// System Error Messsage												( Input Mode Is UIOnly )
	Production,			// Reward, Level Up, Success enchant, etc...							( Input Mode Is GameAndUI )
	Modal,				// Modal Popup Layer													( Input Mode Is UIOnly )
	Left_Sheet,			// NPC Interaction UI													( Input Mode Is GameAndUI )
	Right_Sheet,		// Diablo Inventory Layer												( Input Mode Is GameAndUI )
	Bottom_Sheet,		//																		( Input Mode Is GameAndUI )
	Window,				// Contents Window Widget												( Input Mode Is UIOnly )
	MainHUD,			// MainHUD Layer														( Input Mode Is GameOnly )

	Count,
};
*/

UENUM()
enum class EAsyncWidgetState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};


UCLASS(meta = (DisableNativeTick))
class NAMELESSUISYSTEM_API UNLGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(const FGameplayTag& LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, UCommonActivatableWidget*)> StateFunc);
	UCommonActivatableWidget* PushWidgetToLayerStack(const FGameplayTag& LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitInstanceFunc);
	void RemoveWidgetToLayerStack(UCommonActivatableWidget* InWidget);

	UCommonActivatableWidget* GetTopWidget(const FGameplayTag& InLayerType) const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION(BlueprintCallable)
	void RegistGameLayer(FGameplayTag InLayerType, UNLGameLayer* InLayer);

	// InputSubSystem Callback
	void OnDetectedTouch();
	void OnDetectedMouseAndKeyboard();
	void OnDetectedGamepad();

	// EcpLayer CallBack
	void OnChangedDisplayedWidget(UCommonActivatableWidget* InWidget, UNLGameLayer* InLayer, bool bIsActivated);
	void RefreshGameLayerInputMode();

	UCommonActivatableWidgetContainerBase* GetLayout(const FGameplayTag& InLayer); 

protected:
	TMap<FGameplayTag, UNLGameLayer*> Layers;
	TMap<FGameplayTag, bool> InputModeChecker;
};
