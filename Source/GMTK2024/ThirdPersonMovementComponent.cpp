// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMovementComponent.h"

UThirdPersonMovementComponent::UThirdPersonMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {




}

FRotator UThirdPersonMovementComponent::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const {

	if (unAimTimer > 0.0f) {

		return GetWorld()->GetFirstPlayerController()->GetControlRotation();

	}

	if (Velocity.SquaredLength() > UE_KINDA_SMALL_NUMBER) {
		return Velocity.GetSafeNormal().Rotation();
	}

	return CurrentRotation;
}

