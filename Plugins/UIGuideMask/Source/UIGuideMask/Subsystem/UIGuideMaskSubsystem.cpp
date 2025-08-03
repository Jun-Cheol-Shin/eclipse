// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskSubsystem.h"
#include "../UMG/UIGuideLayer.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UUIGuideMaskSubsystem::ShowGuide(APlayerController* InController, const FGameplayTag& InTag)
{
	bool bSuccessCreated = false;
	if (nullptr == GuideLayer)
	{
		CreateLayer(InController);
	}

	ShowGuide(InTag);
}

void UUIGuideMaskSubsystem::ShowGuide(const FGameplayTag& InTag)
{
	if (nullptr != GuideLayer)
	{
		FGeometry ViewportGeo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);

		if (Widgets.Contains(InTag))
		{
			GuideLayer->Set(ViewportGeo, Widgets[InTag].TargetWidget, Widgets[InTag].GuideParameters);
		}
	}
}

void UUIGuideMaskSubsystem::CreateLayer(APlayerController* InController)
{
	if (LayerClass.ToSoftObjectPath().IsValid()) return;
	TSubclassOf<UUIGuideLayer> LayerSubClass = LayerClass.LoadSynchronous();

	if (nullptr != LayerSubClass)
	{
		GuideLayer = CreateWidget<UUIGuideLayer>(InController, LayerSubClass);
	}
}

void UUIGuideMaskSubsystem::RegistGuideWidget(const FGuideData& InData)
{
	Widgets.Emplace(InData.GameplayTag, InData);

	FGameplayTag* PeekTag = WaitQueue.Peek();
	if (nullptr == PeekTag) return;

	if (Widgets.Contains(*PeekTag))
	{
		ShowGuide(*PeekTag);
	}
}

bool UUIGuideMaskSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UUIGuideMaskSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//if (LayerClass.ToSoftObjectPath().IsValid()) return;
	//TSubclassOf<UUIGuideLayer> LayerSubClass = LayerClass.LoadSynchronous();
}

void UUIGuideMaskSubsystem::Deinitialize()
{
	LayerClass.Reset();
	GuideLayer = nullptr;
}
