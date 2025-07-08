// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpGameLayout.h"
#include "EcpLayer.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

UCommonActivatableWidgetContainerBase* UEcpGameLayout::GetLayout(EEclipseGameLayer InLayer)
{
	/*if (Layers.Contains(InLayer))
	{
		return Layers.FindRef(InLayer);
	}*/

	return nullptr;
}

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
		// Find LayerType

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
			InLayer->OnCompleteDisplayedWidget;
			Layers.Emplace(InLayerType, InLayer);
		}
	}
}
