// Fill out your copyright notice in the Description page of Project Settings.


#include "WelderComponent.h"

#include "PlayerCharacter.h"
#include "PartBase.h"

UWelderComponent::UWelderComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {







}


void UWelderComponent::BeginPlay() {

	blueprintActor = GetWorld()->SpawnActor<ASkeletalMeshActor>();

	blueprintActor->SetHidden(true);


}

void UWelderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);




}




void UWelderComponent::WeldInput() {

	if (!inProgressWeld) {
		
		FVector eyeLoc;
		FRotator eyeRot;

		OwningPlayer->GetActorEyesViewPoint(eyeLoc, eyeRot);

		TArray<FHitResult> hits;

		GetWorld()->LineTraceMultiByChannel(hits, eyeLoc, eyeLoc + (eyeRot.Vector() * 1000.0f), ECC_Visibility);


		for (FHitResult& hit : hits) {
			
			APartBase* partFromHit = Cast<APartBase>(hit.GetActor());


			//if the player is bleuprinting and firing at any of the ship parts
			if (partFromHit && !blueprintActor->IsHidden()) {

				inProgressWeld = GetWorld()->SpawnActor<APartBase>(equippedPart);
				inProgressWeld->SetActorTransform(blueprintActor->GetActorTransform());

				inProgressWeld->weldTarget = partFromHit;
				break;
			}
			
		}
		

	}
	else {

		bool weldDone = inProgressWeld->ProgressWeld();

		if (weldDone) {

			inProgressWeld->SolidifyWeld();

			inProgressWeld = nullptr;

		}

	}



}

void UWelderComponent::BlueprintInput() {

	FVector eyeLoc;
	FRotator eyeRot;

	OwningPlayer->GetActorEyesViewPoint(eyeLoc, eyeRot);

	TArray<FHitResult> hits;

	GetWorld()->LineTraceMultiByChannel(hits, eyeLoc, eyeLoc + (eyeRot.Vector() * 1000.0f), ECC_Visibility);


	for (FHitResult& hit : hits) {

		APartBase* partFromHit = Cast<APartBase>(hit.GetActor());

		if (partFromHit) {

			blueprintActor->SetActorLocation(hit.ImpactPoint);
			blueprintActor->SetActorRotation(hit.ImpactNormal.Rotation());

			

			if (blueprintActor->IsHidden()) {

				blueprintActor->SetHidden(false);

			}

		}

	}


}





void UWelderComponent::SetEquippedPart(PartType type) {

	switch (type) {

		case PartType::PT_STRUCTURAL:
			equippedPart = structuralPart;
			break;

		case PartType::PT_FIREPOWER:
			equippedPart = firepowerPart;
			break;

		case PartType::PT_THRUST:
			equippedPart = thrustPart;
			break;

		case PartType::PT_ENERGY:
			equippedPart = energyPart;
			break;

		case PartType::PT_MISC:
			equippedPart = miscPart;
			break;

	}

	blueprintActor->SetHidden(true);

	USkeletalMeshComponent* blueprintMeshComp = blueprintActor->GetSkeletalMeshComponent();

	blueprintMeshComp->SetSkeletalMeshAsset(equippedPart.GetDefaultObject()->mesh->GetSkeletalMeshAsset());

	//set the display item to only have the holo material
	int nMat = blueprintMeshComp->GetNumMaterials();


	for (int i = 0; i < nMat; i++) {

		blueprintMeshComp->SetMaterial(i, HoloMaterial);

	}


}

void UWelderComponent::SetPartTypePart(PartType type, TSubclassOf<APartBase> part) {

	switch (type) {

	case PartType::PT_STRUCTURAL:
		structuralPart = part;
		break;

	case PartType::PT_FIREPOWER:
		firepowerPart = part;
		break;

	case PartType::PT_THRUST:
		thrustPart = part;
		break;

	case PartType::PT_ENERGY:
		energyPart = part;
		break;

	case PartType::PT_MISC:
		miscPart = part;
		break;

	}

}
