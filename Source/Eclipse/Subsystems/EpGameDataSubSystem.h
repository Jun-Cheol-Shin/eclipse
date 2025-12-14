// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EpResourceSubSystem.h"

#include "../DataTable/EclipseRowBase.h"

#include "EpGameDataSubsystem.generated.h"

/**
 * 
 */
class UEpResourceSubsystem;
class UDataTable;

UCLASS()
class ECLIPSE_API UEpGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	template<typename T = FEclipseRowBase /* FTableRowBase */>
	const T* GetGameData(int32 GameDataId)
	{
		const UScriptStruct* RowStruct = T::StaticStruct();
		//FString RowName = RowStruct->GetName();
		//RowName.RemoveFromEnd(TEXT("Row"));

		FTopLevelAssetPath Path = RowStruct->GetStructPathName();

		if (ensureAlways(DataTables.Contains(Path)))
		{
			UDataTable* DataTable = DataTables.FindRef(Path);
			/*const T* FoundRow = DataTable->FindRow<T>(FName(FString::FormatAsNumber(GameDataId)), FString(""));

			if (nullptr == FoundRow)
			{
				//UE_LOG(LogTemp, Warning, TEXT("UDataTable::FindRow : requested row '%s' not in DataTable '%s'."), Path.GetAssetName(), *GetNameSafe(DataTable));
				return nullptr;
			}

			return FoundRow;*/

			TArray<T*> AllRows;
			DataTable->GetAllRows(FString(""), OUT AllRows);

			const T* const* FoundRow = AllRows.FindByPredicate([GameDataId](T* InRow) -> bool
				{
					return InRow->Id == GameDataId;
				});

			if (FoundRow && *FoundRow)
			{
				return *FoundRow;
			}
		}

		return nullptr;
	}

	template<typename T = FEclipseRowBase /* FTableRowBase */>
	bool GetGameDataList(int32 GameDataId, OUT TArray<const T*>& OutDataList)
	{
		const UScriptStruct* RowStruct = T::StaticStruct();
		//FString RowName = RowStruct->GetName();
		//RowName.RemoveFromEnd(TEXT("Row"));

		FTopLevelAssetPath Path = RowStruct->GetStructPathName();

		if (ensureAlways(DataTables.Contains(Path)))
		{
			UDataTable* DataTable = DataTables.FindRef(Path);
			DataTable->GetAllRows(FString(""), OUT OutDataList);

			if (false == OutDataList.IsEmpty())
			{
				OutDataList = OutDataList.FilterByPredicate([GameDataId](const T* InRow) -> bool
					{
						return InRow->Id == GameDataId;
					});

				return true;
			}

			return false;
		}

		return false;
	}


private:
	template<typename T = FEclipseRowBase>
	void AddGameData(UEpResourceSubsystem* InResourceManager)
	{
		const UScriptStruct* RowStruct = T::StaticStruct();
		//FString RowName = RowStruct->GetName();           // ex) "ItemDatRow -> ItemData"
		//RowName.RemoveFromEnd(TEXT("Row"));

		UDataTable* NewDataTable = InResourceManager->GetDataTable<T>();
		if (ensureAlways(nullptr != NewDataTable))
		{
			DataTables.Emplace(RowStruct->GetStructPathName(), NewDataTable);
		}
	}
	
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	TMap<FTopLevelAssetPath, TObjectPtr<UDataTable>> DataTables;
};
