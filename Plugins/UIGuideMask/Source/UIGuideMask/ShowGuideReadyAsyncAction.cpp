// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowGuideReadyAsyncAction.h"
#include "UIGuideMaskFunctionLibrary.h"
#include "Subsystem/UIGuideMaskSubsystem.h"

UShowGuideReadyAsyncAction* UShowGuideReadyAsyncAction::ShowGuide(const UObject* InWorldContextObject, FGameplayTag InTag, FGuidePredicateSignature InPredicateCondition, float InTimeout, float InInterval)
{
	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!ensureAlwaysMsgf(IsValid(InWorldContextObject), TEXT("World Context was not valid.")))
	{
		return nullptr;
	}

	UShowGuideReadyAsyncAction* NewNode = NewObject<UShowGuideReadyAsyncAction>();
	NewNode->ContextWorld = ContextWorld;
	NewNode->GuideTag = InTag;
	NewNode->PredicateDelegate = InPredicateCondition;
	NewNode->Timeout = InTimeout;
	NewNode->Interval = InInterval;

	return NewNode;
}

void UShowGuideReadyAsyncAction::Activate()
{	
	if (nullptr == GetWorld()) return;

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (nullptr == GameInstance) return;

	UUIGuideMaskSubsystem* Subsystem = GameInstance->GetSubsystem<UUIGuideMaskSubsystem>();
	if (nullptr == Subsystem) return;

	UWidget* OuterWidget = nullptr;
	UWidget* TargetWidget = nullptr;
	Subsystem->GetOuterWidget(&OuterWidget, GuideTag);
	Subsystem->GetTargetWidget(&TargetWidget, GuideTag);

	Target = TargetWidget;
	Outer = OuterWidget;

	if (false == Target.IsValid() || false == Outer.IsValid())
	{
		return;
	}

	if (PredicateDelegate.IsBound() && PredicateDelegate.Execute(Outer.Get(), Target.Get()))
	{
		OnGuideStart();
	}

	else
	{
		StartTime = FPlatformTime::Seconds();

		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &UShowGuideReadyAsyncAction::Tick), Interval);
	}
}


void UShowGuideReadyAsyncAction::Cancel()
{
	Super::Cancel();

	OnGuideCancel();
}

bool UShowGuideReadyAsyncAction::Tick(float DeltaSeconds)
{
	if (Timeout > 0.f && (FPlatformTime::Seconds() - StartTime) >= Timeout)
	{
		OnGuideTimeout();
		return false;
	}


	else if (PredicateDelegate.IsBound() && PredicateDelegate.Execute(Outer.Get(), Target.Get()))
	{
		OnGuideStart();
		return false;
	}


	return true;
}

void UShowGuideReadyAsyncAction::OnGuideStart()
{
	if (nullptr == GetWorld())
	{
		return;
	}

	UUIGuideMaskFunctionLibrary::ShowGuideWidget(GetWorld()->GetGameInstance(), GuideTag);
	OnStarted.Broadcast(GuideTag);

	Clear();
}

void UShowGuideReadyAsyncAction::OnGuideCancel()
{
	OnCancelled.Broadcast(GuideTag);
	Clear();
}

void UShowGuideReadyAsyncAction::OnGuideTimeout()
{
	OnTimeout.Broadcast(GuideTag);
	Clear();
}


void UShowGuideReadyAsyncAction::Clear()
{
	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		TickerHandle.Reset();
	}

	Target.Reset();
	Outer.Reset();

	ContextWorld.Reset();
}