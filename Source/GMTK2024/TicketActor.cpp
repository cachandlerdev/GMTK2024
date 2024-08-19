// Fill out your copyright notice in the Description page of Project Settings.


#include "TicketActor.h"

// Sets default values
ATicketActor::ATicketActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATicketActor::BeginPlay()
{
	Super::BeginPlay();
	
	ticketTime = 0.0f;

}

// Called every frame
void ATicketActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ticketTime += DeltaTime;

}

