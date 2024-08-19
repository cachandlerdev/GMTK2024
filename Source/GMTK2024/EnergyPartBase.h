// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartBase.h"
#include "EnergyPartBase.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2024_API AEnergyPartBase : public APartBase
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AEnergyPartBase(const FObjectInitializer& ObjectInitializer);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
