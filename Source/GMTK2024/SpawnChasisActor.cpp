// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnChasisActor.h"

#include "Kismet/GameplayStatics.h"

#include "PartBase.h"

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
	if (gameMode->shipUnderConstruction)
	{
		return;
	}
	gameMode->currentShipChassis = GetWorld()->SpawnActor<APartBase>(chassisClass);
	gameMode->DoNewShipChassisProcedure();
}
