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
class ECLIPSE_API AEpDropItemActor : public AActor
{
	GENERATED_BODY()

public:
	const UEclipseInventoryItem* GetItemInstance() const { return ItemData.IsValid() ? ItemData.Get() : nullptr; }
	
public:	
	// Sets default values for this actor's properties
	AEpDropItemActor();

	void Set(UEclipseInventoryItem* InItem);
	void Reset();


private:
	void OnInteract(APlayerController* InOwningController);
	void OnPing();
	void OnUseDirect();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void PostInitializeComponents() override;
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
	TWeakObjectPtr<UEclipseInventoryItem> ItemData = nullptr;
};
