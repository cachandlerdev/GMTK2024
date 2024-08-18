// Fill out your copyright notice in the Description page of Project Settings.


#include "EnginePartBase.h"
#include "DataTypes.h"



AEnginePartBase::AEnginePartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	type = PartType::PT_THRUST;

}


void AEnginePartBase::ActivatePart() {




}



void AEnginePartBase::BeginPlay() {
	Super::BeginPlay();

	


}

