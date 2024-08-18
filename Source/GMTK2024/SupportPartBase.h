// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartBase.h"
#include "SupportPartBase.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2024_API ASupportPartBase : public APartBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};