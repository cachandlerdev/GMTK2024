// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasisPartBase.h"

#include "DataTypes.h"


AChasisPartBase::AChasisPartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	type = PartType::PT_CHASSIS;

}


void AChasisPartBase::BeginPlay() {
	Super::BeginPlay();

	


}


void AChasisPartBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


	if (launched) {

		physicsBox->SetAngularDamping(physicsBox->GetAngularDamping() * (1.0f - (0.1f * DeltaTime)));

	}




}