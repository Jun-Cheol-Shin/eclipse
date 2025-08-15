// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskSubsystem.h"
#include "../UMG/UIGuideLayer.h"
#include "../UIGuideMaskFunctionLibrary.h"
#include "../Interface/UIGuideMaskable.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UUIGuideMaskSubsystem::OnStartGuide()
{
	if (true == CurrentGuidedTag.GetTagName().IsNone()) return;
	else if (false == Widgets.Contains(CurrentGuidedTag)) return;

	TWeakObjectPtr<UWidget> OuterWidget = Widgets[CurrentGuidedTag].OuterWidget;
	TWeakObjectPtr<UWidget> TargetWidget = Widgets[CurrentGuidedTag].TargetWidget;
	if (false == OuterWidget.IsValid() || nullptr == OuterWidget.Get()->GetClass()) return;

	if (IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(OuterWidget.Get()))
	{
		MaskableWidget->NativeOnStartGuide(CurrentGuidedTag, TargetWidget.Get());
	}

	else if (OuterWidget.Get()->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
	{
		IUIGuideMaskable::Execute_OnStartGuide(OuterWidget.Get(), CurrentGuidedTag, TargetWidget.Get());
	}
}

void UUIGuideMaskSubsystem::OnCompleteAction()
{
	if (true == CurrentGuidedTag.GetTagName().IsNone()) return;
	else if (!ensure(Widgets.Contains(CurrentGuidedTag))) return;

	TWeakObjectPtr<UWidget> OuterWidget = Widgets[CurrentGuidedTag].OuterWidget;
	if (false == OuterWidget.IsValid()) return;

	FGuideBoxActionParameters ActionParam = Widgets[CurrentGuidedTag].GuideParameters.AcitonParameter;

	FGameplayTag Temp = CurrentGuidedTag;
	TWeakObjectPtr<UWidget> TempTarget = Widgets[CurrentGuidedTag].TargetWidget;
	CompleteGuide();

	if (IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(OuterWidget.Get()))
	{
		MaskableWidget->NativeOnCompleteAction(Temp, TempTarget.Get());
	}

	else if (OuterWidget.Get()->GetClass()->ImplementsInterface(UUIGuideMaskable::StaticClass()))
	{
		IUIGuideMaskable::Execute_OnCompleteAction(Cast<UObject>(OuterWidget.Get()), Temp, TempTarget.Get());
	}

}

void UUIGuideMaskSubsystem::OnViewportResized(FViewport* Viewport, uint32 Unused)
{
	if (nullptr != GuideLayer)
	{
		if (false == CurrentGuidedTag.GetTagName().IsNone() && Widgets.Contains(CurrentGuidedTag))
		{
			FGeometry ViewportGeo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
			GuideLayer->Set(ViewportGeo, Widgets[CurrentGuidedTag].TargetWidget.Get(), Widgets[CurrentGuidedTag].GuideParameters);
		}
	}
}

void UUIGuideMaskSubsystem::ShowGuide(APlayerController* InController, const FGameplayTag& InTag)
{
	if (nullptr == GuideLayer)
	{
		CreateLayer(InController);
	}

	if (nullptr != GuideLayer)
	{
		// Play Animation..

		GuideLayer->AddToViewport(12000);
		ShowGuide(InTag);
	}
}

void UUIGuideMaskSubsystem::ShowGuideSteps(APlayerController* InController, const TArray<FGameplayTag>& InTags)
{
	for (int i = 0; i < InTags.Num(); ++i)
	{
		Steps.Enqueue(InTags[i]);
	}

	FGameplayTag* FirstTag = Steps.Peek();


	if (nullptr != FirstTag)
	{
		// Play Animation.. (FadeIn)

		if (nullptr != GuideLayer)
		{
			GuideLayer->AddToViewport(12000);
			ShowGuide(*FirstTag);
		}

		Steps.Pop();
	}

	else if (nullptr != GuideLayer && GuideLayer->IsInViewport())
	{
		CompleteGuide();
	}
}

void UUIGuideMaskSubsystem::ShowGuide(const FGameplayTag& InTag)
{
	if (ensure(GuideLayer))
	{
		FGeometry ViewportGeo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);


		if (Widgets.Contains(InTag) && ensure(Widgets[InTag].TargetWidget.IsValid()))
		{
			UWidget* TargetWidget = Widgets[InTag].TargetWidget.Get();
			UWidget* OuterWidget = Widgets[InTag].OuterWidget.Get();

			if (UUIGuideMaskFunctionLibrary::IsContainerWidget(TargetWidget))
			{
				TargetWidget = UUIGuideMaskFunctionLibrary::GetEntry(OuterWidget, TargetWidget);
			}


			CurrentGuidedTag = InTag;
			GuideLayer->Set(ViewportGeo, TargetWidget, Widgets[InTag].GuideParameters);
		}
	}
}

void UUIGuideMaskSubsystem::CompleteGuide()
{
	if (FGameplayTag* NextTag = Steps.Peek())
	{
		ShowGuide(*NextTag);
		Steps.Pop();
	}

	else
	{
		Steps.Empty();
		CurrentGuidedTag = FGameplayTag::EmptyTag;

		if (nullptr != GuideLayer && GuideLayer->IsInViewport())
		{
			GuideLayer->RemoveFromParent();
		}

	}
}

void UUIGuideMaskSubsystem::CreateLayer(APlayerController* InController)
{
	if (false == LayerClass.ToSoftObjectPath().IsValid()) return;
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

	WaitQueue.Pop();
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

	FViewport::ViewportResizedEvent.AddUObject(this, &UUIGuideMaskSubsystem::OnViewportResized);
}

void UUIGuideMaskSubsystem::Deinitialize()
{
	FViewport::ViewportResizedEvent.RemoveAll(this);
	LayerClass.Reset();
	GuideLayer = nullptr;
}

bool UUIGuideMaskSubsystem::GetTargetWidget(OUT UWidget** OutTarget, FGameplayTag InTag)
{
	if (Widgets.Contains(InTag))
	{
		*OutTarget = Widgets[InTag].TargetWidget.Get();
		return nullptr != OutTarget && nullptr != *OutTarget;
	}

	return false;
}

bool UUIGuideMaskSubsystem::GetOuterWidget(OUT UWidget** OutOuterWidget, FGameplayTag InTag)
{
	if (Widgets.Contains(InTag))
	{
		*OutOuterWidget = Widgets[InTag].OuterWidget.Get();
		return nullptr != OutOuterWidget && nullptr != *OutOuterWidget;
	}

	return false;
}
