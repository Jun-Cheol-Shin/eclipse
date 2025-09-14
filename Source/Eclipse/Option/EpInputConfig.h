// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EpInputConfig.generated.h"


/*
USTRUCT(BlueprintType)
struct FEpInputAction
{
	GENERATED_BODY()

public:
	FEpInputAction() = default;
	FEpInputAction(UInputAction* InAction, FGameplayTag InTag) : InputAction(InAction), GameplayTag(InTag) {};
	virtual ~FEpInputAction() = default;

public:
	UPROPERTY(EditAnywhere)
	UInputAction* InputAction = nullptr;

	UPROPERTY(EditAnywhere)
	FGameplayTag GameplayTag = {};
};
*/

UCLASS()
class ECLIPSE_API UEpInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
	
public:
	UEpInputConfig(const FObjectInitializer& ObjectInitializer) {};

	// InputTag에 맞는 NativeInputAction을 리턴
	UFUNCTION(BlueprintCallable)
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	// InputTag에 맞는 AbilityInputAction을 리턴
	UFUNCTION(BlueprintCallable)
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// 잘 바뀌지 않는 액션들. ex) move, 카메라 이동 등
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TMap<FGameplayTag, UInputAction*> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TMap<FGameplayTag, UInputAction*> AbilityInputActions;
	
};
