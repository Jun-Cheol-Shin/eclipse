// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWidgetRegistryAsset.h"
#include "EcpUserWidget.h"

void ULayerWidgetRegistryAsset::PostLoad()
{
	Super::PostLoad();

	ReverseRegistryList.Reset();
	for (auto& [LayerType, WidgetList] : RegistryWidgets)
	{
		for (auto& Widget : WidgetList.WidgetSoftPtrList)
		{
			ReverseRegistryList.Emplace(Widget->StaticClass()->GetPathName(), { LayerType, Widget });
		}
	}
}

bool ULayerWidgetRegistryAsset::GetWidget(const FString& InPath, OUT EEclipseGameLayer& OutLayerType, OUT TSoftClassPtr<UEcpUserWidget>* OutSoftPtr)
{
	if (ensure(ReverseRegistryList.Contains(InPath)))
	{
		*OutSoftPtr = ReverseRegistryList.FindRef(InPath).Value;
		OutLayerType = ReverseRegistryList.FindRef(InPath).Key;

		return true;
	}

	return false;
}