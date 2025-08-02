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

	friend class UUIGuideRegistrar;


protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;


private:
	void RegistGuideWidget(FGameplayTag InTag, const UWidget* InWidget) { Widgets.Emplace(InTag, InWidget); }
	void UnregistGuideWidget(FGameplayTag InTag) { if (Widgets.Contains(InTag)) Widgets.Remove(InTag); }
	

private:
	TMap<FGameplayTag, const UWidget*> Widgets;
	
};
