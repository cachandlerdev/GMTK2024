// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Animation/SkeletalMeshActor.h"

#include "SpawnChasisActor.generated.h"


class APartBase;

UCLASS()
class GMTK2024_API ASpawnChasisActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnChasisActor();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APartBase> chassisClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ASkeletalMeshActor* shipHolderMagnet;

	UPROPERTY(EditAnywhere)
		int shipBayShipID = 0;

	UPROPERTY(EditAnywhere)
		AActor* shipBayLocationActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
	void SpawnChassis();
};
