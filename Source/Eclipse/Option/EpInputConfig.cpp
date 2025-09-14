// Fill out your copyright notice in the Description page of Project Settings.


#include "EpInputConfig.h"

const UInputAction* UEpInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	if (NativeInputActions.Contains(InputTag))
	{
		return NativeInputActions[InputTag];
	}

	return nullptr;
}

const UInputAction* UEpInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	if (AbilityInputActions.Contains(InputTag))
	{
		return AbilityInputActions[InputTag];
	}

	return nullptr;
}
