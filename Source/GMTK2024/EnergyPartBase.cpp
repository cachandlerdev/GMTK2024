// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyPartBase.h"

#include "DataTypes.h"


AEnergyPartBase::AEnergyPartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	type = PartType::PT_ENERGY;

}

void AEnergyPartBase::BeginPlay() {
	Super::BeginPlay();

	


}