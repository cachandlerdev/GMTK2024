// Fill out your copyright notice in the Description page of Project Settings.


#include "TicketBoardActor.h"

#include "TicketActor.h"

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


	pluggedTicket = ticket;

	PlugTicketInBP();

}


ATicketActor* ATicketBoardActor::UnplugTicket() {
	
	if (!unpluggable) {

		return nullptr;

	}


	pluggedTicket = nullptr;

	return pluggedTicket;


}


ATicketActor* ATicketBoardActor::GetPluggedTicket() {

	return pluggedTicket;

}
