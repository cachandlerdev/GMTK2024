// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartBase.h"
#include "EnginePartBase.generated.h"

class AMyGameMode;

/**
 * 
 */
UCLASS()
class GMTK2024_API AEnginePartBase : public APartBase
{
	GENERATED_BODY()

public:
	AEnginePartBase(const FObjectInitializer& ObjectInitializer);
	

	UPROPERTY(BlueprintReadOnly)
		float thrust = 100.0f;


	UPROPERTY()
		AMyGameMode* gameMode;

	virtual void ActivatePart() override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
