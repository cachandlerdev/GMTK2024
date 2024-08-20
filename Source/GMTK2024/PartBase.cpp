// Fill out your copyright notice in the Description page of Project Settings.


#include "PartBase.h"

#include "Kismet/GameplayStatics.h"

#include "MyGameMode.h"

#include "ChasisPartBase.h"

#include "TicketActor.h"

// Sets default values
APartBase::APartBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	physicsBox = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(physicsBox);


	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	mesh->SetupAttachment(physicsBox);
}

// Called when the game starts or when spawned
void APartBase::BeginPlay()
{
	Super::BeginPlay();
	localTime = 0.0f;

	weldTime = 0.0f;
	localTimeAtLastWeldCheckin = 0.0f;

	mesh->SetCollisionProfileName("BlockAll");
	//mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//mesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
}

// Called every frame
void APartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	localTime += DeltaTime;

	//DrawDebugSphere(GetWorld(), physicsBox->GetCenterOfMass(), 80.0f, 32, FColor::Green);
	//DrawDebugLine(GetWorld(), FVector::ZeroVector, physicsBox->GetCenterOfMass(), FColor::Green);
	if (physicsBox->IsSimulatingPhysics())
	{
		//DrawDebugLine(GetWorld(), FVector::ZeroVector, physicsBox->GetCenterOfMass(), FColor::Green);

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green,
		                                 "CENTER OF MASS: " + physicsBox->GetCenterOfMass().ToString());
	}
}

bool APartBase::ProgressWeld()
{
	float dt = localTime - localTimeAtLastWeldCheckin;
	localTimeAtLastWeldCheckin = localTime;

	weldTime += dt;

	return (weldTime >= weldTimeRequirement);
}

void APartBase::SolidifyWeld()
{
	if (weldTarget)
	{
		AMyGameMode* gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		owningTicket = weldTarget->owningTicket;
		weldTarget->owningTicket->shipChassis->childParts.Add(this);


		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Weld Target valid");

		FAttachmentTransformRules attachRules = FAttachmentTransformRules::KeepWorldTransform;


		AttachToComponent(weldTarget->mesh, attachRules, "weldSocket");
	}
}


void APartBase::ActivatePart()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, GetName() + " Launched");

	ActivatePartBP();
}
