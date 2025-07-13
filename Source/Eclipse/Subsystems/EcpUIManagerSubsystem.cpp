// Fill out your copyright notice in the Description page of Project Settings.

#include "EcpUIManagerSubsystem.h"
#include "../UI/LayerWidgetRegistryAsset.h"
#include "Engine/StreamableManager.h"

void UEcpUIManagerSubsystem::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
    // After UGameInstance::AddLocalPlayer()

	//LocalPlayer->OnPlatformUserIdChanged();
	//LocalPlayer->OnControllerIdChanged();

	LocalPlayer->OnPlayerControllerChanged().AddUObject(this, &UEcpUIManagerSubsystem::OnPlayerControllerChanged);
	OnPlayerControllerChanged(LocalPlayer->GetPlayerController(GetWorld()));
}

void UEcpUIManagerSubsystem::OnPlayerControllerChanged(APlayerController* InController)
{
	if (nullptr == InController) return;

	NotifyPlayerRemoved(InController->GetLocalPlayer());
	if (FRootViewportLayoutInfo* LayoutInfo = PlayerViewportGameLayouts.Find(InController->GetPlatformUserId()))
	{
		AddLayoutInViewport(LayoutInfo->LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = true;
	}

	else
	{
		CreateGameLayout(InController);
	}

}

void UEcpUIManagerSubsystem::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
    // Called after UGameInstance::RemoveLocalPlayer()

	if (!ensure(LocalPlayer)) return;

	LocalPlayer->OnPlatformUserIdChanged().RemoveAll(this);
	LocalPlayer->OnControllerIdChanged().RemoveAll(this);
	LocalPlayer->OnPlayerControllerChanged().RemoveAll(this);

	if (FRootViewportLayoutInfo* LayoutInfo = PlayerViewportGameLayouts.Find(LocalPlayer->GetPlatformUserId()))
	{
		LayoutInfo->bAddedToViewport = false;
		
		TObjectPtr<UEcpGameLayout> GameLayout = LayoutInfo->RootLayout;

		if (nullptr != GameLayout)
		{
			GameLayout->RemoveFromParent();
			UE_LOG(LogTemp, Log, TEXT("Player [%s]'s root layout [%s] has been removed from the viewport, but other references to its underlying Slate widget still exist. Noting in case we leak it."), *GetNameSafe(LocalPlayer), *GetNameSafe(GameLayout));
		}

		PlayerViewportGameLayouts.Remove(LocalPlayer->GetPlatformUserId());
	}
}

bool UEcpUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UEcpUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize UIManager."));

	PlayerViewportGameLayouts.Reset();
	//TSubclassOf<ULayerWidgetRegistryAsset> RegistryAssetSubClass = WidgetRegistryAssetSoftPtr.LoadSynchronous();

	if (WidgetRegistryAssetSoftPtr.ToSoftObjectPath().IsValid())
	{
		RegistryAsset = WidgetRegistryAssetSoftPtr.LoadSynchronous();

		/*if (ensure(RegistryAsset && RegistryAsset->GetLayerWidgetNum(EEclipseGameLayer::System) > 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success"));
		}*/
	}

}

void UEcpUIManagerSubsystem::Deinitialize()
{
	RegistryAsset = nullptr;

	for (auto& [PlatformId, Layout] : PlayerViewportGameLayouts)
	{
		TObjectPtr<UEcpGameLayout> GameLayout = Layout.RootLayout;
		if (GameLayout)
		{
			GameLayout->RemoveFromParent();
		}
	}

	PlayerViewportGameLayouts.Empty();

	UE_LOG(LogTemp, Warning, TEXT("Deinitialize UIManager."));
}

void UEcpUIManagerSubsystem::CreateGameLayout(APlayerController* InNewController)
{
	if (!ensure(InNewController)) return;
	
	TSubclassOf<UEcpGameLayout> LayoutWidgetClass = LayoutClass.LoadSynchronous();
	if (LayoutWidgetClass)
	{
		UEcpGameLayout* NewLayoutWidget = CreateWidget<UEcpGameLayout>(InNewController, LayoutWidgetClass);

		FRootViewportLayoutInfo NewLayoutInfo;
		NewLayoutInfo.LocalPlayer = InNewController->GetLocalPlayer();
		NewLayoutInfo.RootLayout = NewLayoutWidget;

		PlayerViewportGameLayouts.Emplace(InNewController->GetPlatformUserId(), NewLayoutInfo);

		AddLayoutInViewport(InNewController->GetLocalPlayer(), NewLayoutWidget);

		UE_LOG(LogTemp, Log, TEXT("Player [%s]'s root layout [%s] Added from the viewport, but other references to its underlying Slate widget still exist. Noting in case we leak it."), *GetNameSafe(NewLayoutInfo.LocalPlayer), *GetNameSafe(NewLayoutWidget));
	}
	
}

void UEcpUIManagerSubsystem::AddLayoutInViewport(ULocalPlayer* InLocalPlayer, UEcpGameLayout* InLayout)
{
	if (nullptr != InLayout)
	{
		InLayout->SetPlayerContext(FLocalPlayerContext(InLocalPlayer));
		InLayout->AddToPlayerScreen(1000);

		if (InLocalPlayer->IsPrimaryPlayer())
		{
			FSlateApplication::Get().SetUserFocusToGameViewport(0);
			UE_LOG(LogTemp, Log, TEXT("Set User Focus : IsPrimaryPlayer %s "), ANSI_TO_TCHAR(__FUNCTION__));
		}
	}

}

bool UEcpUIManagerSubsystem::GetOwningLayoutInfo(OUT FRootViewportLayoutInfo& OutInfo) const
{
	if (false == ensure(GetGameInstance())) return false;

	ULocalPlayer* MyLocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (ensure(MyLocalPlayer))
	{
		if (PlayerViewportGameLayouts.Contains(MyLocalPlayer->GetPlatformUserId()))
		{
			OutInfo = PlayerViewportGameLayouts.FindRef(MyLocalPlayer->GetPlatformUserId());

			return true;
		}

	}

	return false;
}

void UEcpUIManagerSubsystem::ShowLayerWidget(EEclipseGameLayer InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass, FOnCompleteLoadedWidgetSignature InCompleteLoadedFunc)
{
	FRootViewportLayoutInfo MyLayoutInfo;
	if (false == GetOwningLayoutInfo(OUT MyLayoutInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Owning Layout Info %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	if (ensure(MyLayoutInfo.RootLayout))
	{
		TSharedPtr<FStreamableHandle> Handle = MyLayoutInfo.RootLayout->PushWidgetToLayerStackAsync(InLayerType, true, InWidgetClass, [InCompleteLoadedFunc](EAsyncWidgetState InState, UCommonActivatableWidget* InLoadingWidget)
			{
				switch (InState)
				{
				case EAsyncWidgetState::Canceled:
				{
					UE_LOG(LogTemp, Warning, TEXT("Cancel Load Widget."));
				}
				break;

				case EAsyncWidgetState::Initialize:
				{
					UE_LOG(LogTemp, Warning, TEXT("Initialize Load Widget. %s"), *GetNameSafe(InLoadingWidget));
				}
				break;

				case EAsyncWidgetState::AfterPush:
				{
					UE_LOG(LogTemp, Warning, TEXT("Complete Load Widget. %s"), *GetNameSafe(InLoadingWidget));

					if (InCompleteLoadedFunc.IsBound())
					{
						InCompleteLoadedFunc.ExecuteIfBound(Cast<UEcpUserWidget>(InLoadingWidget));
					}
				}
				break;

				default:
					break;
				}

			});

		if (Handle.IsValid() == false)
		{
			// Already Loaded?
		}

		else if (Handle->HasLoadCompletedOrStalled())
		{
			// Complete Load
		}

		else if (Handle->IsLoadingInProgress())
		{
			// During Load
		}
	}
}

void UEcpUIManagerSubsystem::HideLayerWidget(UCommonActivatableWidget* InWidget)
{
	FRootViewportLayoutInfo MyLayoutInfo;
	if (false == GetOwningLayoutInfo(OUT MyLayoutInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Owning Layout Info %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}
	
	if (ensure(MyLayoutInfo.RootLayout))
	{
		MyLayoutInfo.RootLayout->RemoveWidgetToLayerStack(InWidget);
	}
}

void UEcpUIManagerSubsystem::OnChangedPlatformUserId(FPlatformUserId InNewId, FPlatformUserId InOldId)
{
	if (PlayerViewportGameLayouts.Contains(InOldId))
	{
		FRootViewportLayoutInfo OldLayoutInfo = PlayerViewportGameLayouts.FindRef(InOldId);

		PlayerViewportGameLayouts.Remove(InOldId);
		PlayerViewportGameLayouts.Emplace(InNewId, OldLayoutInfo);
	}
}
