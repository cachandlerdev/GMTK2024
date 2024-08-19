// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportPartBase.h"

#include "DataTypes.h"


void ASupportPartBase::BeginPlay() {
	Super::BeginPlay();

	type = PartType::PT_SUPPORT;


}