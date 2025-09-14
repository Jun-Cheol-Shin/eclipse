// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EpPlayerController.generated.h"

class UInputMappingContext;
class UEpInputConfig;
/**
 * 
 */
UCLASS()
class ECLIPSE_API AEpPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FORCEINLINE UEpInputConfig* GetInputConfig() const { return EclipseInputConfig; }
	
protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true", Category = "Input"))
	TObjectPtr<UEpInputConfig> EclipseInputConfig = nullptr;
	
};
