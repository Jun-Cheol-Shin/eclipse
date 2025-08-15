// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "ShowGuideReadyAsyncAction.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FGuidePredicateSignature, UWidget*, OuterWidget/* Implemented Guidemaskable Widget */, UWidget*, TargetWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGuideCallbackSignature, FGameplayTag, InTag);


class UWidget;

UCLASS()
class UIGUIDEMASK_API UShowGuideReadyAsyncAction : public UCancellableAsyncAction
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable) 
	FGuideCallbackSignature OnStarted;
	UPROPERTY(BlueprintAssignable) 
	FGuideCallbackSignature OnTimeout;
	UPROPERTY(BlueprintAssignable) 
	FGuideCallbackSignature OnCancelled;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Guide", DisplayName = "Show Guide Async")
	static UShowGuideReadyAsyncAction* ShowGuide(const UObject* InWorldContextObject, FGameplayTag InTag, FGuidePredicateSignature InPredicateCondition, float InTimeout = 1.f, float InInterval = 0.0f);

	virtual void Activate() override;
	virtual void Cancel() override;

	// Start UObject Functions
	virtual UWorld* GetWorld() const override
	{
		return ContextWorld.IsValid() ? ContextWorld.Get() : nullptr;
	}
	// End UObject Functions
	
private:
	bool Tick(float DeltaSeconds);

	void OnGuideStart();
	void OnGuideTimeout();
	void OnGuideCancel();

	void Clear();

private:
	FTSTicker::FDelegateHandle TickerHandle;

	TWeakObjectPtr<UWidget> Target = nullptr;
	TWeakObjectPtr<UWidget> Outer = nullptr;
	TWeakObjectPtr<UWidget> EntryWidget = nullptr;

	TWeakObjectPtr<UWorld> ContextWorld = nullptr;

	FGameplayTag GuideTag;
	FGuidePredicateSignature PredicateDelegate;

	double StartTime = 0.0;
	float Timeout = 1.0f;
	float Interval = 0.f;
};
