// Fill out your copyright notice in the Description page of Project Settings.


#include "NLGameLayout.h"
#include "NLGameLayer.h"
#include "../Subsystems/Input/NLInputManagerSubsystem.h"
#include "../UI/NLUIStatics.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


void UNLGameLayout::OnChangedInputType(ECommonInputType InType)
{
	switch (InType)
	{
	case ECommonInputType::MouseAndKeyboard:
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Mouse And Keyboard!"));
		}
	}
	break;
	case ECommonInputType::Gamepad:
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Gamepad!"));
		}
	}
	break;
	case ECommonInputType::Touch:
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Touch!"));
		}
	}
	break;
	case ECommonInputType::Count:
		break;
	default:
		break;
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

	if (ensure(VisibleChecker.Contains(*TagPtr)))
	{
		VisibleChecker[*TagPtr] = InLayer->GetVisibility() != ESlateVisibility::Collapsed;
		RefreshGameLayerInputMode();
	}

}


void UNLGameLayout::RefreshGameLayerInputMode()
{
	bool bIsVisible = false;

	for (auto& [Tag, IsVisible] : VisibleChecker)
	{
		if (true == IsVisible)
		{
			bIsVisible = true;
			break;
		}
	}

	if (false == bIsVisible)
	{
		// 모든 UI가 꺼졌을 경우, InputMode를 인게임 모드로 변경해야 함. (일단은 소울류를 생각중... GameOnly로 바꾸자)

		if (APlayerController* PlayerController = GetOwningPlayer())
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);
		}
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
			VisibleChecker.Emplace(InLayerType, false);

			InLayer->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}


void UNLGameLayout::NativeConstruct()
{
	Super::NativeConstruct();

	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (false == ensure(MyLocalPlayer)) return;

	UNLInputManagerSubsystem* InputSubSystem = MyLocalPlayer->GetSubsystem<UNLInputManagerSubsystem>();
	if (ensure(InputSubSystem))
	{
		InputSubSystem->OnInputMethodChangedNative.AddUObject(this, &UNLGameLayout::OnChangedInputType);
	}


}

void UNLGameLayout::NativeDestruct()
{
	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (nullptr == MyLocalPlayer) return;

	UNLInputManagerSubsystem* InputSubSystem = MyLocalPlayer->GetSubsystem<UNLInputManagerSubsystem>();
	if (ensure(InputSubSystem))
	{
		InputSubSystem->OnInputMethodChangedNative.RemoveAll(this);
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
	UCommonActivatableWidgetContainerBase* Layer = GetLayout(LayerName);
	if (!ensure(Layer)) return nullptr;
	
	UCommonActivatableWidget* const* FoundWidget = Layer->GetWidgetList().FindByPredicate([ActivatableWidgetClass](const UCommonActivatableWidget* InWidget) -> bool
		{
			return InWidget->GetClass() == ActivatableWidgetClass;
		});

	if (FoundWidget && *FoundWidget)
	{
		return *FoundWidget;
	}

	else
	{
		return Layer->AddWidget<UCommonActivatableWidget>(ActivatableWidgetClass, InitInstanceFunc);
	}
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