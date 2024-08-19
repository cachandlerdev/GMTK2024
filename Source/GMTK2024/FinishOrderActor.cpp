// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishOrderActor.h"

#include "Kismet/GameplayStatics.h"
#include "MyGameMode.h"

#include "TicketActor.h"
#include "TicketBoardActor.h"

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

void AFinishOrderActor::FinishOrder() {

	

	AMyGameMode* gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != nullptr)
	{

		if (!shipBayTicketBoard->GetPluggedTicket() || gameMode->finishingOrder) {

			return;

		}


		gameMode->DoFinishOrderProcedure(shipBayTicketBoard->UnplugTicket());
	}

	
}