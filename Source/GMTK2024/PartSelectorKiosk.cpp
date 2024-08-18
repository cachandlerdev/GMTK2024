// Fill out your copyright notice in the Description page of Project Settings.


#include "PartSelectorKiosk.h"

#include "Kismet/GameplayStatics.h"

#include "PlayerCharacter.h"

#include "WelderComponent.h"

// Sets default values
APartSelectorKiosk::APartSelectorKiosk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("Box Collider");
	SetRootComponent(BoxCollider);

	kioskMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Kiosk Mesh");


	currentDisplay = CreateDefaultSubobject<USkeletalMeshComponent>("Display Mesh");
	
}

// Called when the game starts or when spawned
void APartSelectorKiosk::BeginPlay()
{
	Super::BeginPlay();

	playerWelder = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->Welder;


	kioskMesh->SetWorldLocation(GetActorLocation() - FVector(0.0f, 0.0f, 40.0f));

	//currentDisplayItem = GetWorld()->SpawnActor<ADisplayItemActor>(ADisplayItemActor::StaticClass());

	//currentDisplayItem->GetMesh()->SetSkeletalMesh(testMesh);

	//currentDisplayItem->SetActorLocation(GetActorLocation());
	//currentDisplayItem->SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));

	//currentDisplay->Activate();

	localTime = 0.0f;

	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Block);
	BoxCollider->SetBoxExtent(kioskMesh->GetLocalBounds().BoxExtent * 0.5f);

	holoLocation = kioskMesh->GetSocketLocation("HologramSocket");

	if (currentDisplay) {
		currentDisplay->SetSkeletalMeshAsset(testMesh);
		currentDisplay->SetWorldLocation(holoLocation);
		currentDisplay->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));


		

	}
}

// Called every frame
void APartSelectorKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	localTime += DeltaTime;

	if (currentDisplay) {

		currentDisplay->SetRelativeRotation(FVector(FMath::Cos(localTime), FMath::Sin(localTime), 0.0f).ToOrientationQuat());
		currentDisplay->SetWorldLocation(holoLocation + (FMath::Sin(localTime) * FVector(0.0f, 0.0f, 7.0f)));

	}
	

}



void APartSelectorKiosk::RotateDisplayItem(float value) {

	

	currentItem += (int)value;

	if (currentItem < 0) {

		currentItem = parts.Num() - 1;

	}


	currentItem = FMath::Modulo(currentItem, parts.Num());

	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Orange, FString::SanitizeFloat(currentItem));

	currentDisplay->SetSkeletalMesh(parts[currentItem].GetDefaultObject()->mesh->GetSkeletalMeshAsset());

	//set the welder to this item
	if (!playerWelder) {

		playerWelder = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->Welder;

	}

	playerWelder->SetPartTypePart(partType, parts[currentItem]);


	//set the display item to only have the holo material
	int nMat = currentDisplay->GetNumMaterials();


	for (int i = 0; i < nMat; i++) {

		currentDisplay->SetMaterial(i, HoloMaterial);

	}

	
	

}