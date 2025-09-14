// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NLGameInstance.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UNLGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	NAMELESSUISYSTEM_API virtual void Init() override;
	NAMELESSUISYSTEM_API virtual void Shutdown() override;


private:
	void OnAddedLocalPlayer(ULocalPlayer* InNewPlayer);
	void OnRemoveLocalPlayer(ULocalPlayer* InRemovedPlayer);

};