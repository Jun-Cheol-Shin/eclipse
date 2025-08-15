// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowGuideReadyAsyncAction.h"
#include "UIGuideMaskFunctionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
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

	if (nullptr == TargetWidget || nullptr == OuterWidget)
	{
		return;
	}

	if (true == UUIGuideMaskFunctionLibrary::IsContainerWidget(Target.Get()))
	{
		EntryWidget = UUIGuideMaskFunctionLibrary::GetEntry(OuterWidget, TargetWidget);
	}

	if (PredicateDelegate.IsBound() && PredicateDelegate.Execute(OuterWidget, TargetWidget))
	{
		OnGuideStart();
	}

	else
	{
		Target = TargetWidget;
		Outer = OuterWidget;

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

	if (EntryWidget.IsValid())
	{
		EntryWidget->ForceLayoutPrepass();

		TSharedRef<SWidget> SlateWidget = EntryWidget.Get()->TakeWidget();
		if (true == SlateWidget->NeedsPrepass())
		{
			return true;
		}

		const FGeometry TargetGeometry = EntryWidget->GetTickSpaceGeometry();
		const bool bSizeOK = (TargetGeometry.GetLocalSize().X > 0.f && TargetGeometry.GetLocalSize().Y > 0.f);

		const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
		const FSlateRect View(ViewportGeometry.GetAbsolutePosition(), ViewportGeometry.LocalToAbsolute(ViewportGeometry.GetLocalSize()));
		const FSlateRect Rect(TargetGeometry.GetAbsolutePosition(), TargetGeometry.LocalToAbsolute(TargetGeometry.GetLocalSize()));
		const bool bVisible = FSlateRect::DoRectanglesIntersect(View, Rect);

		if (false == bSizeOK || false == bVisible)
		{ 
			return true;
		}
	}

	if (false == PredicateDelegate.IsBound())
	{
		OnGuideStart();
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

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			UUIGuideMaskFunctionLibrary::ShowGuideWidget(GetWorld()->GetGameInstance(), GuideTag);
			OnStarted.Broadcast(GuideTag);

			Clear();
		}));

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