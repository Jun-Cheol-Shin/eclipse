// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpGameLayout.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

UCommonActivatableWidgetContainerBase* UEcpGameLayout::GetLayout(EEclipseGameLayer InLayer)
{
	if (Layers.Contains(InLayer))
	{
		return Layers.FindRef(InLayer);
	}

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

void UEcpGameLayout::OnDisplayedWidgetChanged(UCommonActivatableWidget* InWidget)
{
	if (!ensure(InWidget && OnCompleteDisplayedWidget.IsBound())) return;

	for (auto& [Layer, Container] : Layers)
	{
		if (Container->GetWidgetList().Contains(InWidget))
		{
			OnCompleteDisplayedWidget.ExecuteIfBound(Layer, InWidget);
			return;
		}
	}
}

void UEcpGameLayout::OnChangedTransitioning(UCommonActivatableWidgetContainerBase* InLayer, bool bIsTransitioning)
{
	if (true == bIsTransitioning)
	{
		// disable input ?
	}

	else
	{
		// enable input ?
	}

}

void UEcpGameLayout::RegistGameLayer(EEclipseGameLayer InLayer, UCommonActivatableWidgetContainerBase* InContainer)
{
	if (false == IsDesignTime())
	{
		if (Layers.Contains(InLayer))
		{
			if (UCommonActivatableWidgetContainerBase* Container = Layers.FindRef(InLayer))
			{
				Container->OnDisplayedWidgetChanged().RemoveAll(this);
				Container->OnTransitioningChanged.RemoveAll(this);
			}

			Layers.Remove(InLayer);
		}

		if (ensure(InContainer))
		{
			//InContainer->SetTransitionDuration(0.f);

			InContainer->OnDisplayedWidgetChanged().AddUObject(this, &UEcpGameLayout::OnDisplayedWidgetChanged);
			InContainer->OnTransitioningChanged.AddUObject(this, &UEcpGameLayout::OnChangedTransitioning);

			Layers.Emplace(InLayer, InContainer);
		}
	}
}
