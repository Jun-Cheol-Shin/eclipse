// Fill out your copyright notice in the Description page of Project Settings.


#include "NLGameLayout.h"
#include "NLGameLayer.h"
#include "../Subsystems/Input/NLInputManagerSubSystem.h"
#include "../UI/NLUIStatics.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


void UNLGameLayout::OnDetectedTouch()
{
	// todo : change mobile main hud

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Touch!"));
	}
}

void UNLGameLayout::OnDetectedMouseAndKeyboard()
{
	// todo : change pc main hud

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Mouse And Keyboard!"));
	}

}

void UNLGameLayout::OnDetectedGamepad()
{
	// todo : change console main hud

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Gamepad!"));
	}
}

void UNLGameLayout::OnChangedDisplayedWidget(UCommonActivatableWidget* InWidget, UNLGameLayer* InLayer, bool bIsActivated)
{
	const FGameplayTag* TagPtr = Layers.FindKey(InLayer);
	if (false == ensure(TagPtr)) return;

	if (false == bIsActivated && InLayer->IsOnlyOneWidget(InWidget))
	{
		InLayer->SetVisibility(ESlateVisibility::Collapsed);
	}

	else
	{
		InLayer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (ensure(InputModeChecker.Contains(*TagPtr)))
	{
		InputModeChecker[*TagPtr] = InLayer->GetVisibility() != ESlateVisibility::Collapsed;
		RefreshGameLayerInputMode();
	}

}

void UNLGameLayout::RefreshGameLayerInputMode()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (nullptr == PlayerController) return;

	bool bActivatedUILayer = false;

	for (auto& [Tag, IsVisible] : InputModeChecker)
	{
		if (false == ensure(Layers.Contains(Tag))) continue;

		UNLGameLayer* Layer = Layers.FindRef(Tag);
		if (false == ensure(Layer)) continue;

		if (true == IsVisible)
		{
			switch (Layer->GetInputMode())
			{
			case ELayerInputMode::GameAndUI:
			{
				bActivatedUILayer = true;
			}
				break;

			case ELayerInputMode::UIOnly:
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->SetShowMouseCursor(true);
				return;
			}
				break;

			default:
			case ELayerInputMode::GameOnly:
				break;
			}

			if (bActivatedUILayer)
			{
				break;
			}
		}
	}

	if (bActivatedUILayer)
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->SetShowMouseCursor(true);
	}

	else
	{
		// All Layer is collapsed or Activated GameOnly Layer.
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		//FSlateApplication::Get().SetUserFocusToGameViewport(0);

	}
}

void UNLGameLayout::RegistGameLayer(FGameplayTag InLayerType, UNLGameLayer* InLayer)
{
	if (false == IsDesignTime())
	{
		if (Layers.Contains(InLayerType))
		{
			if (UNLGameLayer* Layer = Layers.FindRef(InLayerType))
			{
				Layer->OnCompleteDisplayedWidgetDelegate.RemoveAll(this);
			}

			Layers.Remove(InLayerType);
		}

		if (ensure(nullptr != InLayer))
		{
			InLayer->OnCompleteDisplayedWidgetDelegate.AddUObject(this, &UNLGameLayout::OnChangedDisplayedWidget);
			Layers.Emplace(InLayerType, InLayer);
			InputModeChecker.Emplace(InLayerType, false);

			InLayer->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UNLGameLayout::NativeConstruct()
{
	Super::NativeConstruct();

	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (false == ensure(MyLocalPlayer)) return;

	UNLInputManagerSubSystem* InputSubSystem = MyLocalPlayer->GetSubsystem<UNLInputManagerSubSystem>();
	if (ensure(InputSubSystem))
	{
		InputSubSystem->OnChangedDetectTouch.AddUObject(this, &UNLGameLayout::OnDetectedTouch);
		InputSubSystem->OnChangedDetectMouseAndKeyboard.AddUObject(this, &UNLGameLayout::OnDetectedMouseAndKeyboard);
		InputSubSystem->OnChangedDetectGamePad.AddUObject(this, &UNLGameLayout::OnDetectedGamepad);
	}


}

void UNLGameLayout::NativeDestruct()
{
	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (nullptr == MyLocalPlayer) return;

	UNLInputManagerSubSystem* InputSubSystem = MyLocalPlayer->GetSubsystem<UNLInputManagerSubSystem>();
	if (ensure(InputSubSystem))
	{
		InputSubSystem->OnChangedDetectGamePad.RemoveAll(this);
		InputSubSystem->OnChangedDetectMouseAndKeyboard.RemoveAll(this);
		InputSubSystem->OnChangedDetectTouch.RemoveAll(this);
	}

	Super::NativeDestruct();
}

TSharedPtr<FStreamableHandle> UNLGameLayout::PushWidgetToLayerStackAsync(const FGameplayTag& LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, UCommonActivatableWidget*)> StateFunc)
{
	UNLUIStatics::SetInputLock(GetOwningLocalPlayer(), true);

	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
		[this, LayerName, ActivatableWidgetClass, StateFunc]()
		{
			UCommonActivatableWidget* Widget = PushWidgetToLayerStack(LayerName, ActivatableWidgetClass.Get(), [StateFunc](UCommonActivatableWidget& WidgetToInit)
				{
					StateFunc(EAsyncWidgetState::Initialize, &WidgetToInit);
				});

			UNLUIStatics::SetInputLock(GetOwningLocalPlayer(), false);
			StateFunc(EAsyncWidgetState::AfterPush, Widget);
		})
	);
	

	StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
		[this, StateFunc]()
		{
			UNLUIStatics::SetInputLock(GetOwningLocalPlayer(), false);
			StateFunc(EAsyncWidgetState::Canceled, nullptr);
		}));

	return StreamingHandle;
}

UCommonActivatableWidget* UNLGameLayout::PushWidgetToLayerStack(const FGameplayTag& LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitInstanceFunc)
{
	if (UCommonActivatableWidgetContainerBase* Layer = GetLayout(LayerName))
	{
		return Layer->AddWidget<UCommonActivatableWidget>(ActivatableWidgetClass, InitInstanceFunc);
	}

	return nullptr;
}

void UNLGameLayout::RemoveWidgetToLayerStack(const FGameplayTag& InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetPtr)
{
	if (!ensureAlways(InWidgetPtr)) return;

	if (Layers.Contains(InLayerType))
	{
		UNLGameLayer* Layer = Layers[InLayerType];
		if (!ensureAlways(Layer))
		{
			return;
		}

		UCommonActivatableWidgetContainerBase* Container = Layer->GetActivatableWidgetContainer();
		if (!ensureAlways(Container))
		{
			return;
		}

		for (auto& Widget : Container->GetWidgetList())
		{
			if (!ensureAlways(Widget)) continue;

			const FSoftClassPath ThisPath(Widget->GetClass());
			const FSoftObjectPath TargetPath = InWidgetPtr.ToSoftObjectPath();

			if (ThisPath == TargetPath)
			{
				Container->RemoveWidget(*Widget);
				return;
			}
		}

	}
}

void UNLGameLayout::RemoveWidgetToLayerStack(const FGameplayTag& InLayerType, UCommonActivatableWidget* InWidget)
{
	if (nullptr == InWidget) return;

	if (Layers.Contains(InLayerType))
	{
		UNLGameLayer* Layer = Layers[InLayerType];
		if (!ensureAlways(Layer))
		{
			return;
		}

		UCommonActivatableWidgetContainerBase* Container = Layer->GetActivatableWidgetContainer();
		if (!ensureAlways(Container))
		{
			return;
		}

		Container->RemoveWidget(*InWidget);
	}

}

UCommonActivatableWidget* UNLGameLayout::GetTopWidget(const FGameplayTag& InLayerType) const
{
	if (true == Layers.Contains(InLayerType))
	{
		UNLGameLayer* GameLayer = Layers[InLayerType];
		if (nullptr != GameLayer && nullptr != GameLayer->GetActivatableWidgetContainer())
		{
			return GameLayer->GetActivatableWidgetContainer()->GetActiveWidget();
		}
	}

	return nullptr;
}

UCommonActivatableWidgetContainerBase* UNLGameLayout::GetLayout(const FGameplayTag& InLayer)
{
	if (UNLGameLayer* GameLayer = Layers.FindRef(InLayer))
	{
		return GameLayer->GetActivatableWidgetContainer();
	}

	return nullptr;
}