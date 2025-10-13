// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnhancedInputComponent.h"
#include "Interactable.generated.h"

class UInputAction;
class UInputMappingContext;
class UShapeComponent;

class UEpInputConfig;

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
	// return handle
	virtual void BindAction(const UEpInputConfig* InConfig, UEnhancedInputComponent* InComponent, OUT TMap<uint32, TWeakObjectPtr<const UInputAction>>& OutActions) = 0;

protected:
	virtual void SetContext(UInputMappingContext* InContext);
	void Clear();

protected:

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
	void SetAction(APlayerController* OtherController);
	void RemoveAction(APlayerController* OtherController);

private:
	TMap<uint32, TWeakObjectPtr<const UInputAction>> Handles;
	TWeakObjectPtr<UInputMappingContext> InputContext;
	TWeakObjectPtr<APlayerController> OwningController;
};
