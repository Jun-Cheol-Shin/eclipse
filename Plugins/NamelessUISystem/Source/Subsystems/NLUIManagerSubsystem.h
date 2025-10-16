// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../UI/NLGameLayout.h"
#include "../UI/LayerWidgetRegistryAsset.h"
#include "NLUIManagerSubsystem.generated.h"


DECLARE_DELEGATE_OneParam(FOnCompleteLoadedWidgetSignature, UCommonActivatableWidget*)

class UNLGameLayout;

USTRUCT()
struct FRootViewportLayoutInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UNLGameLayout> RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;

	FRootViewportLayoutInfo() {}
	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UNLGameLayout* InRootLayout, bool bIsInViewport)
		: LocalPlayer(InLocalPlayer)
		, RootLayout(InRootLayout)
		, bAddedToViewport(bIsInViewport)
	{
	}

	bool operator==(const FRootViewportLayoutInfo& InOther) const { return this->LocalPlayer == InOther.LocalPlayer; }

	friend uint32 GetTypeHash(const FRootViewportLayoutInfo& Other)
	{
		return GetTypeHash(Other.LocalPlayer);
	}
};



UCLASS(Config = Game, MinimalAPI, Abstract)
class UNLUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	
public:
	template <typename T = UCommonActivatableWidget>
	void ShowLayerWidget(FOnCompleteLoadedWidgetSignature InCompleteLoadedFunc = FOnCompleteLoadedWidgetSignature())
	{
		if (false == ensure(RegistryAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Registry Asset %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		TSoftClassPtr<UCommonActivatableWidget> WidgetSoftPtr;
		FGameplayTag GameLayerType;

		if (false == RegistryAsset->GetWidget(T::StaticClass()->GetPathName(), OUT GameLayerType, OUT WidgetSoftPtr))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid User Widget In Asset %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		FRootViewportLayoutInfo MyLayoutInfo;
		if (false == GetOwningLayoutInfo(OUT MyLayoutInfo))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Owning Layout Info %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		if (ensure(MyLayoutInfo.RootLayout))
		{
			TSharedPtr<FStreamableHandle> Handle = MyLayoutInfo.RootLayout->PushWidgetToLayerStackAsync(GameLayerType, true, WidgetSoftPtr, [InCompleteLoadedFunc](EAsyncWidgetState InState, T* InLoadingWidget)
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
							InCompleteLoadedFunc.ExecuteIfBound(InLoadingWidget);
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
	template <typename T = UCommonActivatableWidget>
	void HideLayerWidget()
	{
		if (false == ensure(RegistryAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Registry Asset %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		TSoftClassPtr<UCommonActivatableWidget> WidgetSoftPtr;
		EEclipseGameLayer GameLayerType;

		if (false == RegistryAsset->GetWidget(T::StaticClass()->GetPathName(), OUT GameLayerType, OUT WidgetSoftPtr))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid User Widget In Asset %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		FRootViewportLayoutInfo MyLayoutInfo;
		if (false == GetOwningLayoutInfo(OUT MyLayoutInfo))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Owning Layout Info %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}
		
		MyLayoutInfo.RootLayout->RemoveWidgetToLayerStack(GameLayerType, T::StaticClass()->GetPathName());
	}


	NAMELESSUISYSTEM_API void ShowLayerWidget(const FGameplayTag& InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass, FOnCompleteLoadedWidgetSignature InCompleteLoadedFunc = FOnCompleteLoadedWidgetSignature());
	NAMELESSUISYSTEM_API void HideLayerWidget(UCommonActivatableWidget* InWidget);

	NAMELESSUISYSTEM_API UCommonActivatableWidget* GetTopLayerWidget(const FGameplayTag& InLayerType);

	NAMELESSUISYSTEM_API UNLGameLayout* GetPlayerGameLayout(const ULocalPlayer* InPlayer) const;

protected:
	NAMELESSUISYSTEM_API void OnChangedPlatformUserId(FPlatformUserId InNewId, FPlatformUserId InOldId);
	NAMELESSUISYSTEM_API void OnPlayerControllerChanged(APlayerController* InController);

private:
	friend class UNLGameInstance;

	void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);

protected:
	NAMELESSUISYSTEM_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	NAMELESSUISYSTEM_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	NAMELESSUISYSTEM_API virtual void Deinitialize() override;


private:
	void CreateGameLayout(APlayerController* InNewController);
	void AddLayoutInViewport(ULocalPlayer* InLocalPlayer, UNLGameLayout* InLayout);
	bool GetOwningLayoutInfo(OUT FRootViewportLayoutInfo& OutInfo) const;

private:
	UPROPERTY(Transient)
	TMap<int32 /*FPlatformUserId*/, FRootViewportLayoutInfo> PlayerViewportGameLayouts;

	UPROPERTY()
	ULayerWidgetRegistryAsset* RegistryAsset = nullptr;

	UPROPERTY()
	TSoftClassPtr<UNLGameLayout> CachedLayoutClass = nullptr;

protected:
	/*
	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UNLGameLayout> LayoutClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<ULayerWidgetRegistryAsset> WidgetRegistryAssetSoftPtr;
	*/
};