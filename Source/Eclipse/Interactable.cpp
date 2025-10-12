// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "EnhancedInputComponent.h"
#include "Components/ShapeComponent.h"

#include "Eclipse/PlayerCore/EpPlayerController.h"
#include "Eclipse/Option/EpInputConfig.h"
#include "InputActionValue.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.


void IInteractable::NativeOnInteract()
{
	Execute_OnInteract(Cast<UObject>(this));
}

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

	SetAction(PlayerController, GetAction(PlayerController));

	if (IsAutoInteract())
	{

	}
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

	Clear(PlayerController);
}

void IInteractable::SetAction(APlayerController* OtherController, const UInputAction* InInputActionTag)
{
	if (!ensureAlways(InInputActionTag))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Input Action Tag, %s"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	AActor* MyActor = Cast<AActor>(this);
	if (!ensure(MyActor))
	{
		UE_LOG(LogTemp, Error, TEXT("interface is not inherited to actor class!!"))
	}

	if (nullptr == OtherController)
	{
		return;
	}

	MyActor->EnableInput(OtherController);


	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(MyActor->InputComponent))
	{
		EnhancedInputComp->ClearActionBindings();
		InteractBindingHandle = BindInteract(GetAction(OtherController), EnhancedInputComp);

		//auto& Handle = EnhancedInputComp->BindAction(GetAction(OtherController), ETriggerEvent::Started, this, &IInteractable::BeginInteract);
		//InteractBindingHandle = Handle.GetHandle();
	}
}


void IInteractable::Clear(APlayerController* OtherController)
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

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(MyActor->InputComponent))
	{
		EnhancedInputComp->RemoveBindingByHandle(InteractBindingHandle);
	}

	InteractBindingHandle = INDEX_NONE;
	MyActor->DisableInput(OtherController);
}
