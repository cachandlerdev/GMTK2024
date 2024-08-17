// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

AMyGameMode::AMyGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {


	orderSheet = CreateDefaultSubobject<UOrderDataSheet>("Order Sheet");


}


void AMyGameMode::BeginPlay() {
	Super::BeginPlay();

	AddOrder();

}


void AMyGameMode::AddOrder() {

	FOrder newOrder;
	newOrder.firepower = 100.0f * difficulty;

	orderSheet->orders.Add(newOrder);

}

void AMyGameMode::SpawnShipChasis() {

	currentShip = GetWorld()->SpawnActor<AActor>(ChasisActorType);




}