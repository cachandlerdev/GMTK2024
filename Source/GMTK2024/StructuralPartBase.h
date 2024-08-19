// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartBase.h"
#include "StructuralPartBase.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2024_API AStructuralPartBase : public APartBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStructuralPartBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
