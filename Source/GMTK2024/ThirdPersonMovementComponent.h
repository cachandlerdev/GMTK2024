// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ThirdPersonMovementComponent.generated.h"

/**
 *
 */
UCLASS()
class GMTK2024_API UThirdPersonMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:

	UThirdPersonMovementComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite)
		bool bUseCustomDirection;

	UPROPERTY(BlueprintReadWrite)
		FVector customDirection;

	UPROPERTY(BlueprintReadWrite)
		float unAimTimer = 0.0f;


	virtual FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotator, float DeltaTime, FRotator& DeltaRotation) const override;

};
