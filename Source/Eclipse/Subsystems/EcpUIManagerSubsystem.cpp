// Fill out your copyright notice in the Description page of Project Settings.

#include "EcpUIManagerSubsystem.h"
#include "../UI/EcpGameLayout.h"

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
}

void UEcpUIManagerSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize UIManager."));

	for (auto& [PlatformId, Layout] : PlayerViewportGameLayouts)
	{
		TObjectPtr<UEcpGameLayout> GameLayout = Layout.RootLayout;
		if (GameLayout)
		{
			GameLayout->RemoveFromParent();
		}
	}

	PlayerViewportGameLayouts.Empty();
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

void UEcpUIManagerSubsystem::OnChangedPlatformUserId(FPlatformUserId InNewId, FPlatformUserId InOldId)
{
	if (PlayerViewportGameLayouts.Contains(InOldId))
	{
		FRootViewportLayoutInfo OldLayoutInfo = PlayerViewportGameLayouts.FindRef(InOldId);

		PlayerViewportGameLayouts.Remove(InOldId);
		PlayerViewportGameLayouts.Emplace(InNewId, OldLayoutInfo);
	}
}
