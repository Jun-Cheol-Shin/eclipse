// Fill out your copyright notice in the Description page of Project Settings.


#include "NLGameLayout.h"
#include "NLGameLayer.h"
#include "../Subsystems/Input/NLInputManagerSubSystem.h"
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
		/*
		*   TODO
			UI에서 모든 위젯을 비활성화하면 범용 UI는 비활성화된 마지막 위젯의 입력 구성을 기본값으로 사용합니다.
			UI의 모든 위젯을 비활성화해야 하는 사용 사례의 경우 마지막으로 비활성화한 위젯이 소프트락 방지를 위해 합리적인 입력 처리 상태를 다시 적용하도록 해야 합니다.
		*/



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

	UNLInputManagerSubSystem* InputSubSystem = MyLocalPlayer->GetSubsystem<UNLInputManagerSubSystem>();
	if (ensure(InputSubSystem))
	{
		InputSubSystem->OnInputMethodChangedNative.AddUObject(this, &UNLGameLayout::OnChangedInputType);
	}


}

void UNLGameLayout::NativeDestruct()
{
	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (nullptr == MyLocalPlayer) return;

	UNLInputManagerSubSystem* InputSubSystem = MyLocalPlayer->GetSubsystem<UNLInputManagerSubSystem>();
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