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

USTRUCT(BlueprintType)
struct FGuideData
{
	GENERATED_BODY()

public:
	FGameplayTag GameplayTag = FGameplayTag();
	UWidget* TargetWidget = nullptr;
	FGuideParameter GuideParameters {};
};


UCLASS(Config = GuideSubsystem)
class UIGUIDEMASK_API UUIGuideMaskSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	
public:
	void ShowGuide(APlayerController* InController, const FGameplayTag& InTag);

private:
	void ShowGuide(const FGameplayTag& InTag);

private:
	friend class UUIGuideRegistrar;

	void RegistGuideWidget(const FGuideData& InData);
	void UnregistGuideWidget(FGameplayTag InTag) { if (Widgets.Contains(InTag)) Widgets.Remove(InTag); }

private:
	void CreateLayer(APlayerController* InController);

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;
	

private:
	TMap<FGameplayTag, FGuideData> Widgets;

	// 비동기 로드가 아직 안된 위젯 대기 큐
	TQueue<FGameplayTag> WaitQueue;
	
private:
	UPROPERTY()
	UUIGuideLayer* GuideLayer = nullptr;

	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UUIGuideLayer> LayerClass;
};
