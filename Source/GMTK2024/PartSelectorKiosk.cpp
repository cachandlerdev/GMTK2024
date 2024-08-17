// Fill out your copyright notice in the Description page of Project Settings.


#include "PartSelectorKiosk.h"

// Sets default values
APartSelectorKiosk::APartSelectorKiosk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("Box Collider");
	SetRootComponent(BoxCollider);

	


	currentDisplay = CreateDefaultSubobject<USkeletalMeshComponent>("Display Mesh");

}

// Called when the game starts or when spawned
void APartSelectorKiosk::BeginPlay()
{
	Super::BeginPlay();




	if (currentDisplay) {

		currentDisplay->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
		currentDisplay->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	}
}

// Called every frame
void APartSelectorKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentDisplay) {

		currentDisplay->SetRelativeRotation((currentDisplay->GetRelativeRotation() + FRotator(0.0f, 0.0f, 10.0f * DeltaTime)).Quaternion());

	}
	

}



void APartSelectorKiosk::RotateDisplayItem(float value) {

	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Orange, FString::SanitizeFloat(value));

	currentItem += (int)value;

	if (currentItem < 0) {

		currentItem = parts.Num() - 1;

	}

	currentItem = FMath::Modulo(currentItem, parts.Num());

	currentDisplay->SetSkeletalMesh(parts[currentItem].GetDefaultObject()->mesh->GetSkeletalMeshAsset());

	

}