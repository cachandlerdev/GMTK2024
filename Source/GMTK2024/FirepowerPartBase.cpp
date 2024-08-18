// Fill out your copyright notice in the Description page of Project Settings.


#include "FirepowerPartBase.h"

#include "DataTypes.h"



// Sets default values
AFirepowerPartBase::AFirepowerPartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	type = PartType::PT_FIREPOWER;

}

void AFirepowerPartBase::BeginPlay() {
	Super::BeginPlay();

	


}