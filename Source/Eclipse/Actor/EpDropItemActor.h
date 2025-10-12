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


protected:
	// IInteractable
	virtual void NativeOnInteract() override;

	virtual bool IsAutoInteract() const override { return false; }

	virtual const UInputAction* GetAction(APlayerController* InOwningController) const override;
	virtual int32 BindInteract(const UInputAction* InAction, UEnhancedInputComponent* InComponent) override;

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
};
