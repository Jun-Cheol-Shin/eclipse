// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "EcpInputManagerSubSystem.generated.h"

/**
 * 
 */

class FEcpInputProcessor;

UCLASS()
class ECLIPSE_API UEcpInputManagerSubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
	
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


private:
	TSharedPtr<FEcpInputProcessor> InputProcessor = nullptr;
};
