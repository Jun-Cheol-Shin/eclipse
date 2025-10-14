// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interactable.h"
#include "EpDropItemActor.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UEclipseInventoryItem;
class UEpInputConfig;

UCLASS()
class ECLIPSE_API AEpDropItemActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEpDropItemActor();

	void Set(const UEclipseInventoryItem* InItem);
	void Reset();

private:
	// Interact
	void OnInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InteractReq(int32 InItemId);



	// End Interact




	void OnPing();
	void OnDirectUse();

protected:
	// IInteractable
	virtual void BindAction(const UEpInputConfig* InConfig, UEnhancedInputComponent* InComponent, OUT TMap<uint32, TWeakObjectPtr<const UInputAction>>& OutActions) override;

	virtual void OnPreInteract_Implementation(AActor* OtherActor) override;
	virtual void OnEndInteract_Implementation(AActor* OtherActor) override;
	// End IInteractable

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* Particle;

private:
	TWeakObjectPtr<const UEclipseInventoryItem> ItemData = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;
};
