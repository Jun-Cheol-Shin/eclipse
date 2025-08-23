// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskSubsystem.h"

#include "../../UIGuideMaskEditor/GuideMaskSettings.h"

#include "../UMG/UIGuideLayer.h"
#include "../UIGuideMaskFunctionLibrary.h"
#include "../Interface/UIGuideMaskable.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UUIGuideMaskSubsystem::ShowGuide(APlayerController* InController, const FGameplayTag& InTag)
{
	if (nullptr == InController) return;

	MyPlayerController = InController;

	if (nullptr == GuideLayer)
	{
		CreateLayer(MyPlayerController.Get());
	}

	if (false == Steps.IsEmpty())
	{
		Steps.Enqueue(InTag);
	}

	else if (nullptr != GuideLayer)
	{
		CurrentGuidedTag = InTag;
		if (false == InputModeSnapshot.IsSnapped())
		{
			SnapshotInputMode(MyPlayerController.Get());
		}

		// Play Animation..
		GuideLayer->AddToViewport(12000);
		ShowGuide(InTag);
	}
}
void UUIGuideMaskSubsystem::ShowGuideSteps(APlayerController* InController, const TArray<FGameplayTag>& InTags)
{
	if (nullptr == InController) return;

	MyPlayerController = InController;

	for (int i = 0; i < InTags.Num(); ++i)
	{
		Steps.Enqueue(InTags[i]);
	}


	if (true == Steps.Dequeue(OUT CurrentGuidedTag))
	{
		if (false == InputModeSnapshot.IsSnapped())
		{
			SnapshotInputMode(MyPlayerController.Get());
		}

		if (nullptr == GuideLayer)
		{
			CreateLayer(MyPlayerController.Get());
		}

		if (nullptr != GuideLayer)
		{
			// Play Animation.. (FadeIn)
			GuideLayer->AddToViewport(12000);
			ShowGuide(CurrentGuidedTag);
		}
	}

	else if (nullptr != GuideLayer && GuideLayer->IsInViewport())
	{
		CompleteGuide();
	}
}

void UUIGuideMaskSubsystem::SetInputType(ECommonInputType InInputType)
{
	InputType = InInputType;

	SetGuideInputType(MyPlayerController.Get());
}


void UUIGuideMaskSubsystem::SetMessage(FGameplayTag InTag, const FGuideMessageParameters& InParameters)
{
	if (CurrentGuidedTag.MatchesTagExact(InTag))
	{
		if (nullptr != GuideLayer)
		{
			GuideLayer->SetGuideTooltip(InParameters);
		}
	}

	else if (Widgets.Contains(InTag))
	{
		if (FGuideData* GuideData = Widgets.Find(InTag))
		{
			GuideData->GuideParameters.MessageParameter = InParameters;
		}
	}
}

void UUIGuideMaskSubsystem::OnStartGuide()
{
	if (!ensure(CurrentGuidedTag.IsValid())) return;
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
	if (!ensure(CurrentGuidedTag.IsValid())) return;
	else if (!ensure(Widgets.Contains(CurrentGuidedTag))) return;

	TWeakObjectPtr<UWidget> OuterWidget = Widgets[CurrentGuidedTag].OuterWidget;
	if (false == OuterWidget.IsValid()) return;

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
		if (false == CurrentGuidedTag.IsValid())
		{
			CompleteGuide();
		}

		else
		{
			ShowGuide(CurrentGuidedTag);
		}
	}
}
void UUIGuideMaskSubsystem::ShowGuide(const FGameplayTag& InTag)
{
	if (ensure(GuideLayer))
	{
		FGeometry ViewportGeo = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(MyPlayerController.Get());

		if (Widgets.Contains(InTag) && ensure(Widgets[InTag].TargetWidget.IsValid()))
		{
			UWidget* TargetWidget = Widgets[InTag].TargetWidget.Get();
			UWidget* OuterWidget = Widgets[InTag].OuterWidget.Get();

			if (UUIGuideMaskFunctionLibrary::IsContainerWidget(TargetWidget))
			{
				TargetWidget = UUIGuideMaskFunctionLibrary::GetEntry(OuterWidget, TargetWidget);
			}

			SetGuideInputType(MyPlayerController.Get());
			GuideLayer->Set(ViewportGeo, TargetWidget, Widgets[InTag].GuideParameters.LayerParameter, Widgets[InTag].GuideParameters.MessageParameter, Widgets[InTag].GuideParameters.bUseAction, Widgets[InTag].GuideParameters.ActionMap.Get(InputType));
		}
	}
}
void UUIGuideMaskSubsystem::CompleteGuide(bool bReleaseQueue)
{
	if (Steps.Dequeue(OUT CurrentGuidedTag))
	{
		ShowGuide(CurrentGuidedTag);
	}

	else
	{
		if (bReleaseQueue)
		{
			Steps.Empty();
		}

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

void UUIGuideMaskSubsystem::SetGuideInputType(APlayerController* InController)
{
	switch (InputType)
	{
	case ECommonInputType::MouseAndKeyboard:
	case ECommonInputType::Touch:
	{
		SetGuideInputTypeInternal(InController, true, GuideLayer->GetBoxWidget());
	}
	break;

	case ECommonInputType::Gamepad:
	{
		SetGuideInputTypeInternal(InController, false, GuideLayer->GetBoxWidget());
	}
	break;
	default:
		break;
	}
}

void UUIGuideMaskSubsystem::SetGuideInputTypeInternal(APlayerController* InController, bool bShowMouseCursor, const TSharedPtr<SWidget>& InWidgetToFocus)
{
	if (!ensure(InController)) return;

	FInputModeGameAndUI Mode;

	if (InWidgetToFocus.IsValid())
	{
		Mode.SetWidgetToFocus(InWidgetToFocus);
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		Mode.SetHideCursorDuringCapture(false);
		InController->SetInputMode(Mode);

		InController->bEnableClickEvents = false;
		InController->SetShowMouseCursor(bShowMouseCursor);

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
			GameViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);
			GameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
		}


		FSlateApplication::Get().SetUserFocus(0, InWidgetToFocus, EFocusCause::SetDirectly);
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}

	else
	{
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InController->SetInputMode(Mode);
		InController->SetShowMouseCursor(bShowMouseCursor);
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

	const UGuideMaskSettings* Settings = GetDefault<UGuideMaskSettings>();
	if (ensure(Settings))
	{
		LayerClass = Settings->GetLayerClass();
		ZOrder = Settings->GetDefaultZOrder();
		InputType = Settings->GetDefaultCommonInputType();
	}

	FViewport::ViewportResizedEvent.AddUObject(this, &UUIGuideMaskSubsystem::OnViewportResized);
}
void UUIGuideMaskSubsystem::Deinitialize()
{
	FViewport::ViewportResizedEvent.RemoveAll(this);

	MyPlayerController.Reset();
	WaitQueue.Empty();
	Widgets.Reset();
	Steps.Empty();
	LayerClass.Reset();

	GuideLayer = nullptr;
}

// Helper
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


void UUIGuideMaskSubsystem::PauseGuide(APlayerController* InController)
{
	TQueue<FGameplayTag> TempQueue;

	if (CurrentGuidedTag.IsValid())
	{
		TempQueue.Enqueue(MoveTemp(CurrentGuidedTag));
	}

	FGameplayTag Tag;
	while (Steps.Dequeue(Tag))
	{
		TempQueue.Enqueue(MoveTemp(Tag));
	}

	Steps.Empty();
	Tag = FGameplayTag::EmptyTag;
	CurrentGuidedTag = FGameplayTag::EmptyTag;


	while (TempQueue.Dequeue(Tag))
	{
		Steps.Enqueue(Tag);
	}

	if (nullptr != GuideLayer && GuideLayer->IsInViewport())
	{
		GuideLayer->RemoveFromParent();
	}

	LoadInputMode(InController);
}
void UUIGuideMaskSubsystem::ResumeGuide(APlayerController* InController)
{
	if (!ensure(GuideLayer)) return;

	if (true == Steps.Dequeue(CurrentGuidedTag))
	{
		if (false == InputModeSnapshot.IsSnapped())
		{
			SnapshotInputMode(InController);
		}

		GuideLayer->AddToViewport(12000);
		ShowGuide(CurrentGuidedTag);
	}
}
