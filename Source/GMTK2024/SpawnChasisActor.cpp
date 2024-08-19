// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnChasisActor.h"

#include "Kismet/GameplayStatics.h"

#include "ChasisPartBase.h"

#include "MyGameMode.h"

// Sets default values
ASpawnChasisActor::ASpawnChasisActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpawnChasisActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpawnChasisActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASpawnChasisActor::SpawnChassis()
{
	AMyGameMode* gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode->currentShipChassis[shipBayShipID])
	{
		return;
	}

	AChasisPartBase* newChassis = GetWorld()->SpawnActor<AChasisPartBase>(chassisClass, shipBayLocationActor->GetActorTransform());


	gameMode->currentShipChassis[shipBayShipID] = newChassis;
	gameMode->DoNewShipChassisProcedure(shipBayShipID);
}
