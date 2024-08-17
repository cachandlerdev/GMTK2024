// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"


#include "Kismet/GameplayStatics.h"

AMyGameMode::AMyGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {


	orderSheet = CreateDefaultSubobject<UOrderDataSheet>("Order Sheet");


}


void AMyGameMode::BeginPlay() {
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(shiftStatTimerHandle, this, &AMyGameMode::ShiftStartCallback, 3.0f);
	

}



void AMyGameMode::ShiftStartCallback() {

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShiftStartAlarm, FVector(0.0f, 0.0f, 0.0f));

	if (shiftStatTimerHandle.IsValid()) {

		shiftStatTimerHandle.Invalidate();

	}

	AddOrder();

}


void AMyGameMode::AddOrder() {

	FOrder newOrder;
	newOrder.firepower = 100.0f * difficulty;

	orderSheet->orders.Add(newOrder);

	if (addOrderTimerHandle.IsValid()) {

		addOrderTimerHandle.Invalidate();

	}

	GetWorld()->GetTimerManager().SetTimer(addOrderTimerHandle, this, &AMyGameMode::AddOrder, 10.0f * (1.0f / difficulty));


}

void AMyGameMode::SpawnShipChasis() {

	currentShip = GetWorld()->SpawnActor<AActor>(ChasisActorType);




}