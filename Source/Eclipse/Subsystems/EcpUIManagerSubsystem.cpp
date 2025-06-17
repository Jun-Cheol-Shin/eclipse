// Fill out your copyright notice in the Description page of Project Settings.

#include "EcpUIManagerSubsystem.h"



void UEcpUIManagerSubsystem::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
    // After UGameInstance::AddLocalPlayer()

	LocalPlayer->OnPlatformUserIdChanged();
	LocalPlayer->OnControllerIdChanged();
	LocalPlayer->OnPlayerControllerChanged();



	/*if (PlayerViewportGameLayouts.Contains())
	{

	}*/


	//LocalPlayer->OnPlayerControllerChanged().RemoveAll(this);


	/*LocalPlayer->OnPlayerControllerSet.AddWeakLambda(this, [this](UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController)
		{
			NotifyPlayerRemoved(LocalPlayer);

			if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
			{
				AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
				LayoutInfo->bAddedToViewport = true;
			}
			else
			{
				CreateLayoutWidget(LocalPlayer);
			}
		});

	if (FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer))
	{
		AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = true;
	}
	else
	{
		CreateLayoutWidget(LocalPlayer);
	}*/
}

void UEcpUIManagerSubsystem::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
    // After UGameInstance::RemoveLocalPlayer()
}

void UEcpUIManagerSubsystem::NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer)
{

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
}

void UEcpUIManagerSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitialize UIManager."));

	PlayerViewportGameLayouts.Reset();
}



void UEcpUIManagerSubsystem::OnChangedPlatformUserId(FPlatformUserId InNewId, FPlatformUserId InOldId)
{
	FLocalPlayerInfo NewInfo(-1, InOldId);

	if (PlayerViewportGameLayouts.Contains(NewInfo))
	{
		UEcpGameLayout* GameLayout = PlayerViewportGameLayouts.FindRef(NewInfo);
		PlayerViewportGameLayouts.Remove(NewInfo);
	}

	//ULocalPlayer* NewLocalPlayer = GetGameInstance()->FindLocalPlayerFromPlatformUserId(InNewId);

}

void UEcpUIManagerSubsystem::OnChangedControllerId(int32 InNewId, int32 InOldId)
{
	ULocalPlayer* NewLocalPlayer = GetGameInstance()->FindLocalPlayerFromControllerId(InNewId);

	NewLocalPlayer->GetCachedUniqueNetId();
}
