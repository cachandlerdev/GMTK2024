// Fill out your copyright notice in the Description page of Project Settings.


#include "PartBase.h"

// Sets default values
APartBase::APartBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");

}

// Called when the game starts or when spawned
void APartBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

