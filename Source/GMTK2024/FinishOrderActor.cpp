// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishOrderActor.h"

#include "Kismet/GameplayStatics.h"
#include "MyGameMode.h"

// Sets default values
AFinishOrderActor::AFinishOrderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFinishOrderActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFinishOrderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFinishOrderActor::FinishOrder(int ticketID) {

	AMyGameMode* gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
	{

		//make sure the ticket and ship exist
		if (ticketID >= gameMode->orderSheet->currentOrders.Num() || shipBayShipID >= gameMode->currentShipChassis.Num()) {

			return;

		}

		//make sure ship isnt nullptr
		if (gameMode->currentShipChassis[shipBayShipID] == nullptr) {

			return;

		}


		gameMode->DoFinishOrderProcedure(shipBayShipID, ticketID);
	}
}