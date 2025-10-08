// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "EpPlayerState.generated.h"


class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ECLIPSE_API AEpPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AEpPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComp; }
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;
	
};
