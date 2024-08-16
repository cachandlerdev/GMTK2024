// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PartComponent_Base.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2024_API UPartComponent_Base : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	// The weight of this part
	UPROPERTY(EditAnywhere)
	float Weight;

	// The cost of this part
	UPROPERTY(EditAnywhere)
	float Cost;

	// Determines whether more tools can be added to this part.
	UPROPERTY(EditAnywhere)
	bool bIsModular;

	// Uses the part's functionality
	UFUNCTION(BlueprintCallable)
	virtual void UsePart();
};
