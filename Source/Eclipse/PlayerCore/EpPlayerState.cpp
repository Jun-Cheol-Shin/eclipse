// Fill out your copyright notice in the Description page of Project Settings.


#include "EpPlayerState.h"
#include "Component/Inventory/EpInventoryComponent.h"
#include "AbilitySystemComponent.h"

AEpPlayerState::AEpPlayerState()
{
	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComp->SetIsReplicated(true);
	AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	InventoryComp = CreateDefaultSubobject<UEpInventoryComponent>("InventoryComponent");
}
