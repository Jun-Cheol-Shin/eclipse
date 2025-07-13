// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EcpGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UEcpGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	virtual void Init() override;
	virtual void Shutdown() override;


private:
	void OnAddedLocalPlayer(ULocalPlayer* InNewPlayer);
	void OnRemoveLocalPlayer(ULocalPlayer* InRemovedPlayer);


};