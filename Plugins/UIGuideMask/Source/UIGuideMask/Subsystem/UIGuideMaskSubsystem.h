// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "../UMG/UIGuideLayer.h"
#include "UIGuideMaskSubsystem.generated.h"


class UWidget;
class UUIGuideLayer;
class APlayerController;
class SWidget;

USTRUCT()
struct FInputModeSnapshot
{
	GENERATED_BODY()

public:
	bool bIsSet = false;

	bool bShowMouseCursor = false;
	bool bEnableClickEvents = false;
	bool bEnableMouseOverEvents = false;

	TWeakPtr<SWidget> FocusWidget = nullptr;

	EMouseLockMode LockMode = EMouseLockMode::DoNotLock;
	EMouseCaptureMode CaptureMode = EMouseCaptureMode::NoCapture;


public:
	void Reset()
	{
		bIsSet = false;
		FocusWidget.Reset();

		bShowMouseCursor = false;
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;

		LockMode = EMouseLockMode::DoNotLock;
		CaptureMode = EMouseCaptureMode::NoCapture;
	}

	bool IsSnapped() const { return bIsSet; }
};




USTRUCT(BlueprintType)
struct FGuideData
{
	GENERATED_BODY()

public:
	FGameplayTag GameplayTag = FGameplayTag();
	FGuideParameter GuideParameters {};

	TWeakObjectPtr<UWidget> OuterWidget = nullptr;
	TWeakObjectPtr<UWidget> TargetWidget = nullptr;

};


UCLASS(Config = UIGuideMask)
class UIGUIDEMASK_API UUIGuideMaskSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UShowGuideReadyAsyncAction;

public:
	void PauseGuide(APlayerController* InController);
	void ResumeGuide(APlayerController* InController);
	void ShowGuide(APlayerController* InController, const FGameplayTag& InTag);
	void ShowGuideSteps(APlayerController* InController, const TArray<FGameplayTag>& InTags);


private:
	void ShowGuide(const FGameplayTag& InTag);
	void CompleteGuide(bool bReleaseQueue = true);
	bool GetTargetWidget(OUT UWidget** OutTarget, FGameplayTag InTag);
	bool GetOuterWidget(OUT UWidget** OutOuterWidget, FGameplayTag InTag);

private:
	friend class UUIGuideRegistrar;
	friend class UUIGuideLayer;

	void OnStartGuide();
	void OnCompleteAction();

	void RegistGuideWidget(const FGuideData& InData);
	void UnregistGuideWidget(FGameplayTag InTag) { if (Widgets.Contains(InTag)) Widgets.Remove(InTag); }

private:
	void SnapshotInputMode(APlayerController* InController);
	void LoadInputMode(APlayerController* InController);
	void SetGuideInputMode(APlayerController* InController, const TSharedPtr<SWidget>& InWidgetToFocus = nullptr);


	void CreateLayer(APlayerController* InController);
	void OnViewportResized(FViewport* Viewport, uint32 Unused);

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;


private:
	TMap<FGameplayTag, FGuideData> Widgets;
	FGameplayTag CurrentGuidedTag = FGameplayTag();

	TQueue<FGameplayTag> Steps;

	// 비동기 로드가 아직 안된 위젯 대기 큐
	TQueue<FGameplayTag> WaitQueue;

	FInputModeSnapshot InputModeSnapshot {};

	bool bKeyboardFocused = false;
	
private:
	UPROPERTY()
	UUIGuideLayer* GuideLayer = nullptr;

	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UUIGuideLayer> LayerClass;
};
