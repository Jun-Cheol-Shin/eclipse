// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../UI/EcpUserWidget.h"
#include "../UI/EcpGameLayout.h"
#include "../UI/LayerWidgetRegistryAsset.h"
#include "EcpUIManagerSubsystem.generated.h"


DECLARE_DELEGATE_OneParam(FOnCompleteLoadedWidgetSignature, UEcpUserWidget*)

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
	template <typename T = UEcpUserWidget>
	void ShowLayerWidget(FOnCompleteLoadedWidgetSignature InCompleteLoadedFunc = FOnCompleteLoadedWidgetSignature())
	{
		if (false == ensure(RegistryAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Registry Asset %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		TSoftClassPtr<UEcpUserWidget> WidgetSoftPtr;
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
	template <typename T = UEcpUserWidget>
	void HideLayerWidget()
	{
		if (false == ensure(RegistryAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Registry Asset %s"), ANSI_TO_TCHAR(__FUNCTION__));
			return;
		}

		TSoftClassPtr<UEcpUserWidget> WidgetSoftPtr;
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


	void ShowLayerWidget(EEclipseGameLayer InLayerType, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass, FOnCompleteLoadedWidgetSignature InCompleteLoadedFunc = FOnCompleteLoadedWidgetSignature());
	void HideLayerWidget(UCommonActivatableWidget* InWidget);

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
	bool GetOwningLayoutInfo(OUT FRootViewportLayoutInfo& OutInfo) const;

private:
	UPROPERTY(Transient)
	TMap<FPlatformUserId, FRootViewportLayoutInfo> PlayerViewportGameLayouts;

	UPROPERTY()
	ULayerWidgetRegistryAsset* RegistryAsset = nullptr;

private:
	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UEcpGameLayout> LayoutClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<ULayerWidgetRegistryAsset> WidgetRegistryAssetSoftPtr;
};