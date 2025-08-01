// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UIGuideMaskSubsystem.generated.h"


class UWidget;

UCLASS()
class UIGUIDEMASK_API UUIGuideMaskSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<UWidget> FindGuideWidget() const;

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;
	
	
};
