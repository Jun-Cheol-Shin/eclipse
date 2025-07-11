// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "Engine/StreamableManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "EcpGameLayout.generated.h"


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
	static UEcpGameLayout* GetGameLayout(APlayerController* InPlayerController);
	static UEcpGameLayout* GetGameLayout(ULocalPlayer* InPlayer);

	
public:
	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(EEclipseGameLayer LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, ActivatableWidgetT*)> StateFunc)
	{
		//static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");
		//const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer) : NAME_None;


		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
			[this, LayerName, ActivatableWidgetClass, StateFunc]()
			{
				// deny input lock
				//UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);


				ActivatableWidgetT* Widget = PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass.Get(), [StateFunc](ActivatableWidgetT& WidgetToInit) 
					{
						StateFunc(EAsyncWidgetState::Initialize, &WidgetToInit);
					});


				// After Load...
				StateFunc(EAsyncWidgetState::AfterPush, Widget);
			})
		);

		StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
			[this, StateFunc]()
			{
				// release input lock
				//UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);

				StateFunc(EAsyncWidgetState::Canceled, nullptr);
			}));

		return StreamingHandle;
	}


	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(EEclipseGameLayer LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		//static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (UCommonActivatableWidgetContainerBase* Layer = GetLayout(LayerName))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}

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
