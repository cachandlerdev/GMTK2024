// Fill out your copyright notice in the Description page of Project Settings.


#include "StructuralPartBase.h"

#include "DataTypes.h"


// Sets default values
AStructuralPartBase::AStructuralPartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	type = PartType::PT_STRUCTURAL;
}


void AStructuralPartBase::BeginPlay()
{
	Super::BeginPlay();
}
