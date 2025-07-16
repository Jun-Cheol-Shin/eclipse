// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWidgetRegistryAsset.h"
#include "CommonUserWidget.h"


void ULayerWidgetRegistryAsset::PostLoad()
{
	Super::PostLoad();

	ReverseRegistryList.Reset();
	for (auto& [Tag, Asset] : RegistryWidgets)
	{
		for (auto& Widget : Asset.WidgetSoftPtrList)
		{
			ReverseRegistryList.Emplace(Widget->StaticClass()->GetPathName(), FRegistWidgetInfo(Tag, Widget));
		}
	}
}

bool ULayerWidgetRegistryAsset::GetWidget(const FString& InPath, OUT FGameplayTag& OutLayerType, OUT TSoftClassPtr<UCommonUserWidget>* OutSoftPtr)
{
	if (ensure(ReverseRegistryList.Contains(InPath)))
	{
		*OutSoftPtr = ReverseRegistryList.FindRef(InPath).WidgetSoftPtr;
		OutLayerType = ReverseRegistryList.FindRef(InPath).LayerTag;

		return true;
	}

	return false;
}