// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "EcpGameLayout.generated.h"


struct FStreamableHandle;
class UCommonActivatableWidgetContainerBase;
class UCommonActivatableWidget;
class UEcpLayer;


UENUM(BlueprintType)
enum class EEclipseGameLayer : uint8
{
	System = 0,			// System Error Messsage
	Production,			// Reward, Level Up, Success enchant, etc...
	Notify,				// Not Stack, It's a Queue Container. ex. In Game Message, Megaphone)
	Modal,				// Modal Popup Layer
	Left_Sheet,			// NPC Interaction UI
	Right_Sheet,		// Diablo Inventory Layer
	Window,				// Main HUD & Contents Window Widget
};

UENUM()
enum class EAsyncWidgetState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};


UCLASS(meta = (DisableNativeTick))
class ECLIPSE_API UEcpGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

	
public:
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(EEclipseGameLayer LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, UCommonActivatableWidget*)> StateFunc);
	UCommonActivatableWidget* PushWidgetToLayerStack(EEclipseGameLayer LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitInstanceFunc);
	void RemoveWidgetToLayerStack(EEclipseGameLayer LayerName, const FString& InWidgetPath);

private:
	UCommonActivatableWidgetContainerBase* GetLayout(EEclipseGameLayer InLayer);

protected:
	void OnChangedDisplayedWidget(UCommonActivatableWidget* InDisplayedWidget, UEcpLayer* InLayer);

protected:
	UFUNCTION(BlueprintCallable)
	void RegistGameLayer(EEclipseGameLayer InLayerType, UEcpLayer* InLayer);
	
private:
	UPROPERTY(Transient)
	TMap<EEclipseGameLayer, UEcpLayer*> Layers;
	
};
