// Fill out your copyright notice in the Description page of Project Settings.


#include "TicketBoardActor.h"

#include "TicketActor.h"

#include "MyGameMode.h"

#include "Kismet/GameplayStatics.h"

#include "ChasisPartBase.h"

// Sets default values
ATicketBoardActor::ATicketBoardActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATicketBoardActor::BeginPlay()
{
	Super::BeginPlay();
	
	pluggedTicket = nullptr;

}

// Called every frame
void ATicketBoardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ATicketBoardActor::PlugTicketIn(ATicketActor* ticket) {

	ticket->AttachToComponent(GetSkeletalMeshComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, "TicketSocket");

	pluggedTicket = ticket;

	PlugTicketInBP();

	if (shipBayActor) {
		ticket->TicketBoard = this;

		AMyGameMode* gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		gameMode->DoNewShipChassisProcedure(chassisClass, ticket, shipBayActor);



	}

}


ATicketActor* ATicketBoardActor::UnplugTicket() {

	if (!unpluggable) {

		return nullptr;

	}

	pluggedTicket->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);


	ATicketActor* temp = pluggedTicket;
	pluggedTicket = nullptr;

	return temp;


}

void ATicketBoardActor::RemoveTicket() {

	pluggedTicket = nullptr;


}



ATicketActor* ATicketBoardActor::GetPluggedTicket() {

	return pluggedTicket;

}
