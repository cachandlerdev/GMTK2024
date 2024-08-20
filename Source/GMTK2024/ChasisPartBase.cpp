// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasisPartBase.h"

#include "DataTypes.h"


AChasisPartBase::AChasisPartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	type = PartType::PT_CHASSIS;
}


void AChasisPartBase::BeginPlay()
{
	Super::BeginPlay();

	childParts.Empty();
	childParts.Add(this);
}


void AChasisPartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (launched)
	{
		physicsBox->SetAngularDamping(physicsBox->GetAngularDamping() * (1.0f - (angularDampingDecay * DeltaTime)));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green,
		                                 "ANG DAMP: " + FString::SanitizeFloat(physicsBox->GetAngularDamping()));

		float speed = GetVelocity().Length();


		if (speed > 1.0f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "COM VEL: " + FString::SanitizeFloat(speed));

			physicsBox->ComponentVelocity = FMath::Lerp(physicsBox->ComponentVelocity,
			                                            speed * FVector(0.0f, -1.0f, 0.0f), DeltaTime * 10.0f);
		}
	}
}
