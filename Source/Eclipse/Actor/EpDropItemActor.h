// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EpDropItemActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

class UEclipseInventoryItem;


UCLASS()
class ECLIPSE_API AEpDropItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEpDropItemActor();

	void Set(const UEclipseInventoryItem* InItem);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* Particle;

private:
	TWeakObjectPtr<const UEclipseInventoryItem> ItemData = nullptr;

private:
#if WITH_EDITOR
	void ShowDebugCollision(const TArray<FString>& Args);
#endif

#if WITH_EDITORONLY_DATA
	IConsoleObject* ConsoleDelHandle;
#endif
};
