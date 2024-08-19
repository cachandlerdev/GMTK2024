// Fill out your copyright notice in the Description page of Project Settings.


#include "EnginePartBase.h"
#include "DataTypes.h"

#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"

#include "ChasisPartBase.h"

#include "TicketActor.h"



AEnginePartBase::AEnginePartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	type = PartType::PT_THRUST;

}


void AEnginePartBase::ActivatePart() {




}



void AEnginePartBase::BeginPlay() {
	Super::BeginPlay();

	
	gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	thrust = baseAttribute;

	

}


void AEnginePartBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


	if (launched) {

		owningTicket->shipChassis->physicsBox->AddForceAtLocation(-GetActorForwardVector() * thrust, GetActorLocation());
		
		//gameMode->currentShipChassis->physicsBox->AddForceAtLocation(-thrustVector * thrust, centerOfThrust);


		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, "Engine Firing");

	}




}

