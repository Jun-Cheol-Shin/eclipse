// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpGameLayout.h"
#include "EcpLayer.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"


/*static*/ UEcpGameLayout* UEcpGameLayout::GetGameLayout(APlayerController* InPlayerController)
{
	// TODO : Get Owning Layout

	return nullptr;
}

/*static*/ UEcpGameLayout* UEcpGameLayout::GetGameLayout(ULocalPlayer* InPlayer)
{
	// TODO : get Another local player layout

	return nullptr;

}


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

UCommonActivatableWidgetContainerBase* UEcpGameLayout::GetLayout(EEclipseGameLayer InLayer)
{
	if (UEcpLayer* EcpLayer = Layers.FindRef(InLayer))
	{
		return EcpLayer->GetActivatableWidgetContainer();
	}

	return nullptr;
}