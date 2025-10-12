// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class UInputAction;
class UShapeComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ECLIPSE_API IInteractable
{
	GENERATED_BODY()

protected:
	virtual bool IsAutoInteract() const = 0;

	// return handle
	virtual int32 BindInteract(const UInputAction* InAction, UEnhancedInputComponent* InComponent) = 0;

	// return input action
	virtual const UInputAction* GetAction(APlayerController* InOwningController) const = 0;

protected:
	virtual void NativeOnInteract();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact", DisplayName = "On Interact")
	void OnInteract();

	UFUNCTION()
	virtual void NativeOnPreInteract(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact", DisplayName = "On Pre Interact")
	void OnPreInteract(AActor* OtherActor);

	UFUNCTION()
	virtual void NativeOnEndInteract(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact", DisplayName = "On End Interact")
	void OnEndInteract(AActor* OtherActor);

private:
	void SetAction(APlayerController* OtherController, const UInputAction* InInputAction);
	void Clear(APlayerController* OtherController);

private:
	int32 InteractBindingHandle = INDEX_NONE;
};
