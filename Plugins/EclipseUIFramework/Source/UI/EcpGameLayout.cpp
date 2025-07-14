// Fill out your copyright notice in the Description page of Project Settings.


#include "EcpGameLayout.h"
#include "EcpLayer.h"
#include "../Subsystems/Input/EcpInputManagerSubSystem.h"
#include "../UI/EcpUIFunctions.h"

#include "CommonUI/Public/Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


void UEcpGameLayout::OnDetectedTouch()
{
	// todo : change mobile main hud

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Touch!"));
	}
}

void UEcpGameLayout::OnDetectedMouseAndKeyboard()
{
	// todo : change pc main hud

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Detected Mouse And Keyboard!"));
	}

}

void UEcpGameLayout::OnChangedDisplayedWidget(UCommonActivatableWidget* InWidget, UEcpLayer* InLayer, bool bIsActivated)
{
	const EEclipseGameLayer* LayerTypePtr = Layers.FindKey(InLayer);
	if (false == ensure(LayerTypePtr)) return;

	if (false == bIsActivated && InLayer->IsOnlyOneWidget(InWidget))
	{
		InLayer->SetVisibility(ESlateVisibility::Collapsed);
	}

	else
	{
		InLayer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	InputModeChecker[(uint8)*LayerTypePtr] = InLayer->GetVisibility() != ESlateVisibility::Collapsed;
	RefreshGameLayerInputMode();
}

void UEcpGameLayout::RefreshGameLayerInputMode()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (nullptr == PlayerController) return;

	bool bActivatedUILayer = false;

	for (int i = 0; i < (uint8)EEclipseGameLayer::Count; ++i)
	{
		EEclipseGameLayer LayerType = static_cast<EEclipseGameLayer>(i);

		if (false == ensure(Layers.Contains(LayerType))) continue;

		UEcpLayer* Layer = Layers.FindRef(LayerType);
		if (false == ensure(Layer)) continue;

		
		if (true == InputModeChecker[i])
		{
			if (true == Layer->IsNeedUIOnlyInputMode())
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->SetShowMouseCursor(true);
				return;
			}

			bActivatedUILayer = true;
			break;
		}
	}

	if (false == bActivatedUILayer)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		FSlateApplication::Get().SetUserFocusToGameViewport(0);
	}

	else
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->SetShowMouseCursor(true);
	}
}

void UEcpGameLayout::RegistGameLayer(EEclipseGameLayer InLayerType, UEcpLayer* InLayer)
{
	if (false == IsDesignTime())
	{
		if (Layers.Contains(InLayerType))
		{
			if (UEcpLayer* Layer = Layers.FindRef(InLayerType))
			{
				Layer->OnCompleteDisplayedWidgetDelegate.RemoveAll(this);
			}

			Layers.Remove(InLayerType);
		}

		if (ensure(nullptr != InLayer))
		{
			InLayer->OnCompleteDisplayedWidgetDelegate.AddUObject(this, &UEcpGameLayout::OnChangedDisplayedWidget);
			Layers.Emplace(InLayerType, InLayer);

			InLayer->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UEcpGameLayout::NativeConstruct()
{
	Super::NativeConstruct();

	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (false == ensure(MyLocalPlayer)) return;

	UEcpInputManagerSubSystem* EcpInputSubSystem = MyLocalPlayer->GetSubsystem<UEcpInputManagerSubSystem>();
	if (ensure(EcpInputSubSystem))
	{
		EcpInputSubSystem->OnChangedDetectTouch.AddUObject(this, &UEcpGameLayout::OnDetectedTouch);
		EcpInputSubSystem->OnChangedDetectMouseAndKeyboard.AddUObject(this, &UEcpGameLayout::OnDetectedMouseAndKeyboard);
	}
}

void UEcpGameLayout::NativeDestruct()
{
	ULocalPlayer* MyLocalPlayer = GetOwningLocalPlayer();
	if (nullptr == MyLocalPlayer) return;

	UEcpInputManagerSubSystem* EcpInputSubSystem = MyLocalPlayer->GetSubsystem<UEcpInputManagerSubSystem>();
	if (ensure(EcpInputSubSystem))
	{
		EcpInputSubSystem->OnChangedDetectMouseAndKeyboard.RemoveAll(this);
		EcpInputSubSystem->OnChangedDetectTouch.RemoveAll(this);
	}

	Super::NativeDestruct();
}

TSharedPtr<FStreamableHandle> UEcpGameLayout::PushWidgetToLayerStackAsync(EEclipseGameLayer LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetState, UCommonActivatableWidget*)> StateFunc)
{
	UEcpUIFunctions::SetInputLock(GetOwningLocalPlayer(), true);

	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
		[this, LayerName, ActivatableWidgetClass, StateFunc]()
		{
			UCommonActivatableWidget* Widget = PushWidgetToLayerStack(LayerName, ActivatableWidgetClass.Get(), [StateFunc](UCommonActivatableWidget& WidgetToInit)
				{
					StateFunc(EAsyncWidgetState::Initialize, &WidgetToInit);
				});

			UEcpUIFunctions::SetInputLock(GetOwningLocalPlayer(), false);
			StateFunc(EAsyncWidgetState::AfterPush, Widget);
		})
	);
	

	StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
		[this, StateFunc]()
		{
			UEcpUIFunctions::SetInputLock(GetOwningLocalPlayer(), false);
			StateFunc(EAsyncWidgetState::Canceled, nullptr);
		}));

	return StreamingHandle;
}

UCommonActivatableWidget* UEcpGameLayout::PushWidgetToLayerStack(EEclipseGameLayer LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(UCommonActivatableWidget&)> InitInstanceFunc)
{
	if (UCommonActivatableWidgetContainerBase* Layer = GetLayout(LayerName))
	{
		return Layer->AddWidget<UCommonActivatableWidget>(ActivatableWidgetClass, InitInstanceFunc);
	}

	return nullptr;
}

void UEcpGameLayout::RemoveWidgetToLayerStack(UCommonActivatableWidget* InWidget)
{
	if (nullptr == InWidget) return;

	for (auto& [LayerType, Layer]  : Layers)
	{
		if (nullptr == Layer) continue;

		UCommonActivatableWidgetContainerBase* Container = Layer->GetActivatableWidgetContainer();
		if (nullptr == Container) continue;

		Container->RemoveWidget(*InWidget);
	}

}

UCommonActivatableWidgetContainerBase* UEcpGameLayout::GetLayout(EEclipseGameLayer InLayer)
{
	if (UEcpLayer* EcpLayer = Layers.FindRef(InLayer))
	{
		return EcpLayer->GetActivatableWidgetContainer();
	}

	return nullptr;
}