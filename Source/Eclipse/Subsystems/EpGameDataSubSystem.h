// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EpResourceSubSystem.h"

#include "Engine/DataTable.h"
// Data Row
#include "../DataTable/ItemDataRow.h"
#include "../DataTable/ItemResourceDataRow.h"

#include "EpGameDataSubSystem.generated.h"

/**
 * 
 */
class UEpResourceSubSystem;
class UDataTable;

UCLASS()
class ECLIPSE_API UEpGameDataSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	template<typename T = FTableRowBase /* FTableRowBase */>
	const T* GetGameData(int32 GameDataId)
	{
		const UScriptStruct* RowStruct = T::StaticStruct();
		FString RowName = RowStruct->GetName();
		RowName.RemoveFromEnd(TEXT("Row"));

		if (ensureAlways(DataTables.Contains(RowName)))
		{
			// find
			UDataTable* DataTable = DataTables[RowName];
			const T* FoundRow = DataTable->FindRow<T>(FName(FString::FormatAsNumber(GameDataId)), FString(""));

			if (nullptr == FoundRow)
			{
				UE_LOG(LogTemp, Warning, TEXT("UDataTable::FindRow : requested row '%s' not in DataTable '%s'."), *RowName, *GetNameSafe(DataTable));
				return nullptr;
			}

			return FoundRow;
		}

		return nullptr;
	}

private:
	template<typename T = FTableRowBase>
	void AddGameData(UEpResourceSubSystem* InResourceManager)
	{
		const UScriptStruct* RowStruct = T::StaticStruct();
		FString RowName = RowStruct->GetName();           // ex) "ItemDatRow -> ItemData"
		RowName.RemoveFromEnd(TEXT("Row"));

		UDataTable* NewDataTable = nullptr;
		InResourceManager->GetDataTable(RowName, OUT &NewDataTable);

		if (ensureAlways(nullptr != NewDataTable))
		{
			DataTables.Emplace(RowName, NewDataTable);
		}
	}
	
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	TMap<FString, TObjectPtr<UDataTable>> DataTables;
};
