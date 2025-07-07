// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcpUIManagerSubsystem.generated.h"


class UEcpGameLayout;


USTRUCT()
struct FRootViewportLayoutInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UEcpGameLayout> RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;

	FRootViewportLayoutInfo() {}
	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UEcpGameLayout* InRootLayout, bool bIsInViewport)
		: LocalPlayer(InLocalPlayer)
		, RootLayout(InRootLayout)
		, bAddedToViewport(bIsInViewport)
	{
	}

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }
};



UCLASS(Config = Game)
class ECLIPSE_API UEcpUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	//void Show()

protected:
	void OnChangedPlatformUserId(FPlatformUserId InNewId, FPlatformUserId InOldId);
	void OnPlayerControllerChanged(APlayerController* InController);

private:
	friend class UEcpGameInstance;

	void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;


private:
	void CreateGameLayout(APlayerController* InNewController);
	void AddLayoutInViewport(ULocalPlayer* InLocalPlayer, UEcpGameLayout* InLayout);


private:
	UPROPERTY(Transient)
	TMap<FPlatformUserId, FRootViewportLayoutInfo> PlayerViewportGameLayouts;

	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UEcpGameLayout> LayoutClass;
	
};
