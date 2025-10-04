// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EpResourceSubSystem.generated.h"



UCLASS()
class ECLIPSE_API UEpResourceSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	friend class UEpGameDataSubSystem;

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool GetDataTable(const FString& InDataName, OUT UDataTable** OutDataTable);
};
