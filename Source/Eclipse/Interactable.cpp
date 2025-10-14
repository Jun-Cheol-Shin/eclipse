// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "EnhancedInputComponent.h"
#include "Components/ShapeComponent.h"

#include "Eclipse/PlayerCore/EpPlayerController.h"
#include "Eclipse/Option/EpInputConfig.h"
#include "InputActionValue.h"

#include "Eclipse/SubSystems/EpInputManagerSubSystem.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.


void IInteractable::NativeOnPreInteract(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Execute_OnPreInteract(Cast<UObject>(this), OtherActor);

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (nullptr == Pawn)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (nullptr == PlayerController)
	{
		return;
	}

	SetAction(PlayerController);
}

void IInteractable::NativeOnEndInteract(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Execute_OnEndInteract(Cast<UObject>(this), OtherActor);

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (nullptr == Pawn)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (nullptr == PlayerController)
	{
		return;
	}

	RemoveAction(PlayerController);
}

void IInteractable::SetContext(UInputMappingContext* InContext)
{
	InputContext = InContext;
}

void IInteractable::SetAction(APlayerController* OtherController)
{
	if (nullptr == OtherController)
	{
		return;
	}

	OwningController = OtherController;

	if (ENetMode::NM_DedicatedServer == OwningController->GetNetMode())
	{
		UE_LOG(LogTemp, Display, TEXT("Dedicated Server : %s"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}


	// Set Input Context.
	ULocalPlayer* LocalPlayer = OtherController->GetLocalPlayer();
	if (nullptr == LocalPlayer)
	{
		return;
	}

	if (UEpInputManagerSubSystem* SubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubSystem>())
	{
		if (InputContext.IsValid())
		{
			SubSystem->AddMappingContext(InputContext.Get(), 0);
		}
	}

	// Enable Actor Input
	AActor* MyActor = Cast<AActor>(this);
	if (!ensure(MyActor))
	{
		UE_LOG(LogTemp, Error, TEXT("interface is not inherited to actor class!!"))
	}

	MyActor->EnableInput(OtherController);


	// Bind Input Action
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(MyActor->InputComponent))
	{
		EnhancedInputComp->ClearActionBindings();

		AEpPlayerController* EpController = Cast<AEpPlayerController>(OtherController);
		
		if (!ensure(EpController))
		{
			return;
		}

		const UEpInputConfig* Config = EpController->GetInputConfig();

		if (ensure(Config))
		{
			BindAction(Config, EnhancedInputComp, OUT Handles);
		}
	}
}


void IInteractable::RemoveAction(APlayerController* OtherController)
{
	AActor* MyActor = Cast<AActor>(this);
	if (!ensure(MyActor))
	{
		UE_LOG(LogTemp, Error, TEXT("interface is not inherited to actor class!!"));
		return;
	}

	if (nullptr == OtherController)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = OtherController->GetLocalPlayer();
	if (nullptr == LocalPlayer)
	{
		return;
	}

	if (UEpInputManagerSubSystem* SubSystem = LocalPlayer->GetSubsystem<UEpInputManagerSubSystem>())
	{
		if (InputContext.IsValid())
		{
			SubSystem->RemoveMappingContext(InputContext.Get());
		}
	}


	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(MyActor->InputComponent))
	{
		for (auto& [Num, Action] : Handles)
		{
			EnhancedInputComp->RemoveActionBindingForHandle(Num);
		}

		Handles.Reset();
	}

	MyActor->DisableInput(OtherController);

	OwningController.Reset();
}


void IInteractable::Clear()
{
	RemoveAction(OwningController.Get());
	InputContext.Reset();
	Handles.Reset();

	OwningController.Reset();
}