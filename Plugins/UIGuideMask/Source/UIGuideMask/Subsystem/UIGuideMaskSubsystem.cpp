// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskSubsystem.h"
#include "../UMG/UIGuideLayer.h"
#include "../Interface/UIGuideMaskable.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UUIGuideMaskSubsystem::OnStartGuide(UWidget* InGuideWidget)
{
	if (!ensure(CurrentGuidedTag.GetTagName().IsNone())) return;
	else if (!ensure(Widgets.Contains(CurrentGuidedTag))) return;

	TWeakObjectPtr<UWidget> OuterWidget = Widgets[CurrentGuidedTag].OuterWidget;
	if (false == OuterWidget.IsValid()) return;


	if (IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(OuterWidget.Get()))
	{
		MaskableWidget->NativeOnStartGuide(CurrentGuidedTag, InGuideWidget);
	}
}

void UUIGuideMaskSubsystem::OnCompletePreAction(UWidget* InGuideWidget)
{
	if (!ensure(CurrentGuidedTag.GetTagName().IsNone())) return;
	else if (!ensure(Widgets.Contains(CurrentGuidedTag))) return;

	TWeakObjectPtr<UWidget> OuterWidget = Widgets[CurrentGuidedTag].OuterWidget;
	if (false == OuterWidget.IsValid()) return;

	FGuideBoxActionParameters ActionParam = Widgets[CurrentGuidedTag].GuideParameters.AcitonParameter;

	switch (ActionParam.ActionPolicy)
	{
	case EActionCompletionPolicy::Immediate:
	{
		if (IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(OuterWidget.Get()))
		{
			MaskableWidget->NativeOnCompleteAction(CurrentGuidedTag, InGuideWidget);
		}

		CompleteGuide();
	}
		break;


	default:
	case EActionCompletionPolicy::OnTargetHandled:
	case EActionCompletionPolicy::OnPredicate:
		break;
	}

}

void UUIGuideMaskSubsystem::OnCompletePostAction(UWidget* InGuideWidget)
{
	if (!ensure(CurrentGuidedTag.GetTagName().IsNone())) return;
	else if (!ensure(Widgets.Contains(CurrentGuidedTag))) return;

	TWeakObjectPtr<UWidget> OuterWidget = Widgets[CurrentGuidedTag].OuterWidget;
	if (false == OuterWidget.IsValid()) return;

	FGuideBoxActionParameters ActionParam = Widgets[CurrentGuidedTag].GuideParameters.AcitonParameter;

	switch (ActionParam.ActionPolicy)
	{
	case EActionCompletionPolicy::OnTargetHandled:
	{
		if (IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(OuterWidget.Get()))
		{
			MaskableWidget->NativeOnCompleteAction(CurrentGuidedTag, InGuideWidget);
		}

		CompleteGuide();
	}
	break;

	case EActionCompletionPolicy::OnPredicate:
	{
		if (IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(OuterWidget.Get()))
		{
			if (true == MaskableWidget->NativeOnEvaluateCompletionPredicate(CurrentGuidedTag))
			{
				MaskableWidget->NativeOnCompleteAction(CurrentGuidedTag, InGuideWidget);
				CompleteGuide();
			}

			else
			{
				TWeakObjectPtr<UWidget> WeakGuideWidget = InGuideWidget;
				TWeakObjectPtr<UWidget> WeakMaskableWidget = OuterWidget;
				
				// wait..
				FTSTicker::GetCoreTicker().AddTicker(
					FTickerDelegate::CreateWeakLambda(this, 
						[this,
						TimeoutSeconds = ActionParam.PredicateTime,
						WeakGuideWidget, 
						WeakMaskableWidget,
						StartTime = FPlatformTime::Seconds()](float InDeltaTime) -> bool
						{
							if (false == WeakMaskableWidget.IsValid() || false == WeakGuideWidget.IsValid()) return false;

							IUIGuideMaskable* MaskableWidget = Cast<IUIGuideMaskable>(WeakMaskableWidget.Get());
							if (nullptr == MaskableWidget)
							{	
								MaskableWidget->NativeOnFailedPredicate(CurrentGuidedTag);
								return false;
							}
							
							if (true == MaskableWidget->NativeOnEvaluateCompletionPredicate(CurrentGuidedTag))
							{
								MaskableWidget->NativeOnCompleteAction(CurrentGuidedTag, WeakGuideWidget.Get());
								CompleteGuide();

								return false;
							}

							if (FPlatformTime::Seconds() - StartTime > TimeoutSeconds)
							{
								MaskableWidget->NativeOnFailedPredicate(CurrentGuidedTag);
								return false;
							}

							MaskableWidget->OnCompletionPredicate(CurrentGuidedTag);

							return true;

						}));

			}
		}
	}
		break;

	default:
	case EActionCompletionPolicy::Immediate:
		break;
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
		// Add Viewport...



		ShowGuide(InTag);
		Steps.Pop();
	}
}

void UUIGuideMaskSubsystem::ShowGuideSteps(APlayerController* InController, const TArray<FGameplayTag>& InTags)
{
	for (int i = 0; i < InTags.Num(); ++i)
	{
		Steps.Enqueue(InTags[i]);
	}




	if (FGameplayTag* FirstTag = Steps.Peek())
	{
		if (nullptr != GuideLayer)
		{
			// Play Animation.. (FadeIn)
			// Add Viewport...



			ShowGuide(*FirstTag);
			Steps.Pop();
		}
	}
}

void UUIGuideMaskSubsystem::ShowGuide(const FGameplayTag& InTag)
{
	if (ensure(GuideLayer))
	{
		FGeometry ViewportGeo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);

		if (Widgets.Contains(InTag) && ensure(Widgets[InTag].TargetWidget.IsValid()))
		{
			CurrentGuidedTag = InTag;
			GuideLayer->Set(ViewportGeo, Widgets[InTag].TargetWidget.Get(), Widgets[InTag].GuideParameters);
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
		CurrentGuidedTag = FGameplayTag::EmptyTag;

		// Hide Layer		(FadeOut)
		// Remove Viewport


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
