// Fill out your copyright notice in the Description page of Project Settings.


#include "EpGameDataSubSystem.h"

void UEpGameDataSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UEpResourceSubSystem* ResourceManager = Collection.InitializeDependency<UEpResourceSubSystem>();

	if (nullptr != ResourceManager)
	{
		AddGameData<FItemDataRow>(ResourceManager);
		//AddGameData<FItemResourceDataRow>(ResourceManager);
	}
}

void UEpGameDataSubSystem::Deinitialize()
{

}
