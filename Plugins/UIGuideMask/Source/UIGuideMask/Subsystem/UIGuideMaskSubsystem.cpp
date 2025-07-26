// Fill out your copyright notice in the Description page of Project Settings.


#include "UIGuideMaskSubsystem.h"

TWeakObjectPtr<UWidget> UUIGuideMaskSubsystem::FindGuideWidget() const
{



	return TWeakObjectPtr<UWidget>();
}

bool UUIGuideMaskSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere
		return ChildClasses.Num() == 0;
	}

	return false;
}

void UUIGuideMaskSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UUIGuideMaskSubsystem::Deinitialize()
{

}
