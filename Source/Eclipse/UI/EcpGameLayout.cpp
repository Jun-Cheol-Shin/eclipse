// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpGameLayout.h"
#include "EcpLayer.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UEcpGameLayout::OnChangedDisplayedWidget(UCommonActivatableWidget* InDisplayedWidget, UEcpLayer* InLayer)
{
	const EEclipseGameLayer* LayerTypePtr = Layers.FindKey(InLayer);

	if (ensure(LayerTypePtr))
	{
		*LayerTypePtr;
		InDisplayedWidget;
	}
}

void UEcpGameLayout::RegistGameLayer(EEclipseGameLayer InLayerType, UEcpLayer* InLayer)
{
	if (false == IsDesignTime())
	{
		if (Layers.Contains(InLayerType))
		{
			if (UEcpLayer* Layer = Layers.FindRef(InLayerType))
			{
				Layer->OnCompleteDisplayedWidget.Unbind();
			}

			Layers.Remove(InLayerType);
		}

		if (ensure(nullptr != InLayer))
		{
			InLayer->OnCompleteDisplayedWidget.BindUObject(this, &UEcpGameLayout::OnChangedDisplayedWidget);
			Layers.Emplace(InLayerType, InLayer);
		}
	}
}

TSharedPtr<FStreamableHandle> UEcpGameLayout::PushWidgetToLayerStackAsync(EEclipseGameLayer LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, UCommonActivatableWidget*)> StateFunc)
{
	//static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");
	//const FName SuspendInputToken = bSuspendInputUntilComplete ? UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer) : NAME_None;


	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
		[this, LayerName, ActivatableWidgetClass, StateFunc]()
		{
			// deny input lock
			//UCommonUIExtensions::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);


			UCommonActivatableWidget* Widget = PushWidgetToLayerStack(LayerName, ActivatableWidgetClass.Get(), [StateFunc](UCommonActivatableWidget& WidgetToInit)
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

UCommonActivatableWidget* UEcpGameLayout::PushWidgetToLayerStack(EEclipseGameLayer LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitInstanceFunc)
{
	//static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

	if (UCommonActivatableWidgetContainerBase* Layer = GetLayout(LayerName))
	{
		return Layer->AddWidget<UCommonActivatableWidget>(ActivatableWidgetClass, InitInstanceFunc);
	}

	return nullptr;
}

void UEcpGameLayout::RemoveWidgetToLayerStack(EEclipseGameLayer LayerName, const FString& InWidgetPath)
{
	//static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

	UCommonActivatableWidgetContainerBase* Layer = GetLayout(LayerName);
	if (false == ensure(Layer)) return;

	UCommonActivatableWidget* RemoveWidget = nullptr;

	for (auto& Widget : Layer->GetWidgetList())
	{
		if (Widget->StaticClass()->GetPathName().Equals(InWidgetPath))
		{
			RemoveWidget = Widget;
			break;
		}
	}

	if (nullptr != RemoveWidget)
	{
		Layer->RemoveWidget(*RemoveWidget);
	}
}

UCommonActivatableWidgetContainerBase* UEcpGameLayout::GetLayout(EEclipseGameLayer InLayer)
{
	if (UEcpLayer* EcpLayer = Layers.FindRef(InLayer))
	{
		return EcpLayer->GetActivatableWidgetContainer();
	}

	return nullptr;
}