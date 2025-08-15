// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskSubsystem.h"
#include "../UMG/UIGuideLayer.h"
#include "../UIGuideMaskFunctionLibrary.h"
#include "../Interface/UIGuideMaskable.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
		if (CurrentGuidedTag.GetTagName().IsNone())
		{
			CompleteGuide();
		}

		else
		{
			ShowGuide(CurrentGuidedTag);
		}
	}
}

void UUIGuideMaskSubsystem::ShowGuide(APlayerController* InController, const FGameplayTag& InTag)
{
	if (nullptr == GuideLayer)
	{
		CreateLayer(InController);
	}

	if (false == Steps.IsEmpty())
	{
		Steps.Enqueue(InTag);
	}

	else if (nullptr != GuideLayer)
	{
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance && false == InputModeSnapshot.IsSnapped())
		{
			SnapshotInputMode(GetGameInstance()->GetFirstLocalPlayerController());
		}

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


	if (false == Steps.IsEmpty())
	{
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance && false == InputModeSnapshot.IsSnapped())
		{
			SnapshotInputMode(GetGameInstance()->GetFirstLocalPlayerController());
		}

		FGameplayTag NewTag;
		Steps.Dequeue(OUT NewTag);

		if (nullptr != GuideLayer)
		{		
			// Play Animation.. (FadeIn)
			GuideLayer->AddToViewport(12000);
			ShowGuide(NewTag);
		}
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

			if (true == Widgets[InTag].GuideParameters.bUseAction)
			{
				SetGuideInputMode(GetGameInstance()->GetFirstLocalPlayerController(), TargetWidget->TakeWidget());
			}

			CurrentGuidedTag = InTag;
			GuideLayer->Set(ViewportGeo, TargetWidget, Widgets[InTag].GuideParameters);
		}
	}
}

void UUIGuideMaskSubsystem::CompleteGuide()
{
	if (false == Steps.IsEmpty())
	{
		FGameplayTag NewTag;
		Steps.Dequeue(OUT NewTag);
		ShowGuide(NewTag);
	}

	else
	{
		Steps.Empty();
		CurrentGuidedTag = FGameplayTag::EmptyTag;

		if (nullptr != GuideLayer && GuideLayer->IsInViewport())
		{
			GuideLayer->RemoveFromParent();
		}

		if (UGameInstance* GameInstance = GetGameInstance())
		{
			LoadInputMode(GetGameInstance()->GetFirstLocalPlayerController());
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

void UUIGuideMaskSubsystem::SnapshotInputMode(APlayerController* InController)
{
	if (!ensure(InController)) return;

	InputModeSnapshot.Reset();
	InputModeSnapshot.bShowMouseCursor = InController->bShowMouseCursor;
	InputModeSnapshot.bEnableClickEvents = InController->bEnableClickEvents;
	InputModeSnapshot.bEnableMouseOverEvents = InController->bEnableMouseOverEvents;

	UGameViewportClient* GameviewportClient = nullptr;
	if (ULocalPlayer* LocalPlayer = InController->GetLocalPlayer())
	{
		GameviewportClient = LocalPlayer->ViewportClient;
	}

	if (nullptr != GameviewportClient)
	{
		InputModeSnapshot.CaptureMode = GameviewportClient->GetMouseCaptureMode();
		InputModeSnapshot.LockMode = GameviewportClient->GetMouseLockMode();
	}

	TSharedPtr<SWidget> Focused = FSlateApplication::Get().GetKeyboardFocusedWidget();
	// (게임패드/유저 포커스 기준)
	if (!Focused.IsValid())
	{
		Focused = FSlateApplication::Get().GetUserFocusedWidget(/*UserIndex=*/0);
	}

	if (nullptr != Focused)
	{
		InputModeSnapshot.FocusWidget = Focused.ToWeakPtr();
	}

}

void UUIGuideMaskSubsystem::LoadInputMode(APlayerController* InController)
{
	if (!ensure(InController)) return;

	InController->SetShowMouseCursor(InputModeSnapshot.bShowMouseCursor);
	InController->bEnableClickEvents = InputModeSnapshot.bEnableClickEvents;
	InController->bEnableMouseOverEvents = InputModeSnapshot.bEnableMouseOverEvents;

	UGameViewportClient* GameViewportClient = nullptr;
	if (InController)
	{
		if (ULocalPlayer* LocalPlayer = InController->GetLocalPlayer())
		{
			GameViewportClient = LocalPlayer->ViewportClient;
		}
	}

	if (nullptr != GameViewportClient)
	{
		GameViewportClient->SetMouseCaptureMode(InputModeSnapshot.CaptureMode);
		GameViewportClient->SetMouseLockMode(InputModeSnapshot.LockMode);
	}

	if (InputModeSnapshot.FocusWidget.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(InputModeSnapshot.FocusWidget.Pin(), EFocusCause::SetDirectly);
	}

	InputModeSnapshot.Reset();
}

void UUIGuideMaskSubsystem::SetGuideInputMode(APlayerController* InController, const TSharedPtr<SWidget>& InWidgetToFocus, bool bUseKeyboardFocus)
{
	if (!ensure(InController)) return;

	FInputModeGameAndUI Mode;

	UGameViewportClient* GameViewportClient = nullptr;
	if (InController)
	{
		if (ULocalPlayer* LocalPlayer = InController->GetLocalPlayer())
		{
			GameViewportClient = LocalPlayer->ViewportClient;
		}
	}

	if (nullptr != GameViewportClient)
	{
		GameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
		GameViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);
	}

	if (true == bUseKeyboardFocus && InWidgetToFocus.IsValid())
	{
		Mode.SetWidgetToFocus(InWidgetToFocus);
	}

	Mode.SetHideCursorDuringCapture(false);

	InController->SetInputMode(Mode);
	InController->bShowMouseCursor = true;
	InController->bEnableClickEvents = true;
	InController->bEnableMouseOverEvents = true;

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
