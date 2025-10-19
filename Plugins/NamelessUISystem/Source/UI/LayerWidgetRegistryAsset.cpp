// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWidgetRegistryAsset.h"
#include "CommonActivatableWidget.h"


void ULayerWidgetRegistryAsset::PostLoad()
{
	Super::PostLoad();

	ReverseRegistryList.Reset();
	for (auto& [Tag, Asset] : RegistryWidgets)
	{
		for (auto& Widget : Asset.WidgetSoftPtrList)
		{
			FString AssetName = Widget.ToSoftObjectPath().GetAssetName();

			AssetName.RemoveFromStart(TEXT("WBP_"));
			AssetName.RemoveFromEnd(TEXT("_C"));

			ReverseRegistryList.Emplace(AssetName, FRegistWidgetInfo(Tag, Widget));
		}
	}
}

/*
void ULayerWidgetRegistryAsset::Initialize()
{
	ReverseRegistryList.Reset();
	for (auto& [Tag, Asset] : RegistryWidgets)
	{
		for (auto& Widget : Asset.WidgetSoftPtrList)
		{
			ReverseRegistryList.Emplace(Widget->GetPathName(), FRegistWidgetInfo(Tag, Widget));
		}
	}
}*/

bool ULayerWidgetRegistryAsset::GetWidget(const FString& InPath, OUT FGameplayTag& OutLayerType, OUT TSoftClassPtr<UCommonActivatableWidget>* OutSoftPtr)
{
	if (ensure(ReverseRegistryList.Contains(InPath)))
	{
		*OutSoftPtr = ReverseRegistryList.FindRef(InPath).WidgetSoftPtr;
		OutLayerType = ReverseRegistryList.FindRef(InPath).LayerTag;

		return true;
	}

	return false;
}