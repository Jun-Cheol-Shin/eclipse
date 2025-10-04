// Fill out your copyright notice in the Description page of Project Settings.


#include "EpPreviewGameMode.h"

#include "../Subsystems/EpGameDataSubSystem.h"

void AEpPreviewGameMode::StartPlay()
{
	Super::StartPlay();

	/*if (UEpGameDataSubSystem* Subsystem = GetGameInstance()->GetSubsystem<UEpGameDataSubSystem>())
	{
		const FItemDataRow* ItemData = Subsystem->GetGameData<FItemDataRow>(0);
	}*/

}
