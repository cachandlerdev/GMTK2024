// Fill out your copyright notice in the Description page of Project Settings.


#include "WelderComponent.h"

#include "PlayerCharacter.h"
#include "PartBase.h"

#include "MyGameMode.h"

#include "Kismet/GameplayStatics.h"

UWelderComponent::UWelderComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void UWelderComponent::BeginPlay()
{
	blueprintActor = GetWorld()->SpawnActor<ASkeletalMeshActor>();

	SetBlueprintActorVisible(false);
	blueprintActor->SetActorEnableCollision(false);

	gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

}

void UWelderComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	buildCooldownTimer += DeltaTime;
}


void UWelderComponent::WeldInput()
{
	if (!equippedPart)
	{
		return;
	}


	if (!inProgressWeld && buildCooldownTimer > buildCooldown)
	{
		//only start a weld if they are aiming a blueprint
		if (blueprintOn)
		{
			FVector eyeLoc;
			FRotator eyeRot;

			OwningPlayer->GetActorEyesViewPoint(eyeLoc, eyeRot);

			TArray<FHitResult> hits;

			GetWorld()->LineTraceMultiByChannel(hits, eyeLoc, eyeLoc + (eyeRot.Vector() * 1000.0f), ECC_Visibility);


			for (FHitResult& hit : hits)
			{
				APartBase* partFromHit = Cast<APartBase>(hit.GetActor());


				//if the player is bleuprinting and firing at any of the ship parts
				if (partFromHit)
				{
					inProgressWeld = GetWorld()->SpawnActor<APartBase>(equippedPart);
					inProgressWeld->SetActorTransform(blueprintActor->GetActorTransform());

					inProgressWeld->weldTarget = partFromHit;
					break;
				}
			}
		}
	}
	else if (inProgressWeld)
	{
		bool weldDone = inProgressWeld->ProgressWeld();

		if (weldDone)
		{
			buildCooldownTimer = 0.0f;
			inProgressWeld->SolidifyWeld();

			gameMode->AddPartToBuild(inProgressWeld, inProgressWeld->owningTicket);

			inProgressWeld = nullptr;
		}
	}
}


void UWelderComponent::WeldReleased()
{
	if (inProgressWeld)
	{
		inProgressWeld->Destroy();

		inProgressWeld = nullptr;
	}
}


void UWelderComponent::BlueprintInput()
{
	if (!equippedPart || inProgressWeld)
	{
		if (blueprintOn)
		{
			SetBlueprintActorVisible(false);
		}

		return;
	}

	FVector eyeLoc;
	FRotator eyeRot;

	OwningPlayer->GetActorEyesViewPoint(eyeLoc, eyeRot);

	TArray<FHitResult> hits;

	GetWorld()->LineTraceMultiByChannel(hits, eyeLoc, eyeLoc + (eyeRot.Vector() * 1000.0f), ECC_Visibility);
	
	bool anyPartHit = false;

	for (FHitResult& hit : hits)
	{
		APartBase* partFromHit = Cast<APartBase>(hit.GetActor());

		if (partFromHit)
		{
			blueprintActor->SetActorLocation(hit.ImpactPoint);
			blueprintActor->SetActorRotation(hit.ImpactNormal.Rotation());


			anyPartHit = true;
			if (!blueprintOn)
			{
				SetBlueprintActorVisible(true);
			}
		}
	}

	//if they look away from the ship and keep holding aim the turn off blueprints
	if (!anyPartHit && blueprintOn)
	{
		SetBlueprintActorVisible(false);
	}
}


void UWelderComponent::BlueprintReleased()
{
	SetBlueprintActorVisible(false);
}

void UWelderComponent::SetBlueprintActorVisible(bool value)
{
	blueprintOn = value;

	blueprintActor->GetSkeletalMeshComponent()->SetVisibility(value);
}

void UWelderComponent::ScrollPartType(float value)
{
	int dir = (int)FMath::Sign(value);

	int cur = static_cast<int>(equippedPartType);

	cur += dir;

	if (cur < 0)
	{
		cur = NUM_PART_TYPES - 1;
	}

	cur %= NUM_PART_TYPES;


	equippedPartType = static_cast<PartType>(cur);

	SetEquippedPart(equippedPartType);
}


void UWelderComponent::SetEquippedPart(PartType type)
{
	switch (type)
	{
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

	case PartType::PT_SUPPORT:
		equippedPart = miscPart;
		break;
	}

	if (!equippedPart)
	{
		return;
	}

	SetBlueprintActorVisible(false);

	USkeletalMeshComponent* blueprintMeshComp = blueprintActor->GetSkeletalMeshComponent();

	blueprintMeshComp->SetSkeletalMeshAsset(equippedPart.GetDefaultObject()->mesh->GetSkeletalMeshAsset());

	//set the display item to only have the holo material
	int nMat = blueprintMeshComp->GetNumMaterials();


	for (int i = 0; i < nMat; i++)
	{
		blueprintMeshComp->SetMaterial(i, HoloMaterial);
	}
}

void UWelderComponent::SetPartTypePart(PartType type, TSubclassOf<APartBase> part)
{
	switch (type)
	{
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

	case PartType::PT_SUPPORT:
		miscPart = part;
		break;
	}

	//update the equipped part
	SetEquippedPart(equippedPartType);
}
