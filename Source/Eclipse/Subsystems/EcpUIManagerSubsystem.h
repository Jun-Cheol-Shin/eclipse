// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EcpUIManagerSubsystem.generated.h"


class UEcpGameLayout;


USTRUCT()
struct FLocalPlayerInfo
{
	GENERATED_BODY()

public:
	FLocalPlayerInfo() {};
	FLocalPlayerInfo(int32 InContId, const FPlatformUserId& InUserId) : ControllerId(InContId), PlatformUserId(InUserId) {}

public:
	int32 ControllerId = -1;
	FPlatformUserId PlatformUserId{};

	bool operator==(const FLocalPlayerInfo& OtherInfo) const { return OtherInfo.ControllerId == this->ControllerId || OtherInfo.PlatformUserId == this->PlatformUserId; }

	friend uint32 GetTypeHash(const FLocalPlayerInfo& OtherInfo)
	{
		uint32 Hash = FCrc::MemCrc32(&OtherInfo, sizeof(FLocalPlayerInfo));
		return Hash;
	}
};



UCLASS()
class ECLIPSE_API UEcpUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:


protected:
	// ULocalPlayer Event
	void OnChangedPlatformUserId(FPlatformUserId InNewId, FPlatformUserId InOldId);
	void OnChangedControllerId(int32 InNewId, int32 InOldId);

private:
	// TODO : friend class UGameInstance........

	void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer);


protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;



private:
	UPROPERTY(Transient)
	TMap<FLocalPlayerInfo, UEcpGameLayout*> PlayerViewportGameLayouts;
	
};
