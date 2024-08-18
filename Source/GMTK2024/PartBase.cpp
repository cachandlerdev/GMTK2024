// Fill out your copyright notice in the Description page of Project Settings.


#include "PartBase.h"

// Sets default values
APartBase::APartBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");

}

// Called when the game starts or when spawned
void APartBase::BeginPlay()
{
	Super::BeginPlay();
	localTime = 0.0f;

	weldTime = 0.0f;
	localTimeAtLastWeldCheckin = 0.0f;

}

// Called every frame
void APartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	localTime += DeltaTime;

}

bool APartBase::ProgressWeld() {

	float dt = localTime - localTimeAtLastWeldCheckin;
	localTimeAtLastWeldCheckin = localTime;

	weldTime += dt;

	return (weldTime >= weldTimeRequirement);

}

void APartBase::SolidifyWeld() {

	if (weldTarget) {

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Weld Target valid");

		FAttachmentTransformRules attachRules = FAttachmentTransformRules::KeepWorldTransform;


		AttachToComponent(weldTarget->mesh, attachRules, "weldSocket");

	}

}