// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuHub_Inventory.h"

#include "../../Common/GridBasedList/GridBasedInventoryList.h"

void UMenuHub_Inventory::NativeOnActiveContent()
{

}

void UMenuHub_Inventory::NativeOnInactiveContent()
{

}

void UMenuHub_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	
#if WITH_EDITOR
	InventoryCheatHandle = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Inventory.AddItem"),
		TEXT("Add Item <Row Size> <Column Size>"),
		FConsoleCommandWithArgsDelegate::CreateWeakLambda(this, [this](const TArray<FString>& InArgs)
			{
				if (nullptr == InventoryList) return;

				if (InArgs.Num() < 2)
				{
					UE_LOG(LogTemp, Warning, TEXT("usage: MyGame.SetValue <Name> <Value>"));
					return;
				}

				int Row = FCString::Atoi(*InArgs[0]);
				int Column = FCString::Atoi(*InArgs[1]);

				if (UGridBasedInventoryItem* NewItem = NewObject<UGridBasedInventoryItem>())
				{
					NewItem->Size = FVector2D(Row, Column);
					InventoryList->AddItem(NewItem);
				}

			}),
		ECVF_Default);
#endif

}

void UMenuHub_Inventory::NativeDestruct()
{
#if WITH_EDITOR
	if (InventoryCheatHandle)
	{
		IConsoleManager::Get().UnregisterConsoleObject(InventoryCheatHandle);
	}
#endif

	Super::NativeDestruct();
}
