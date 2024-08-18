// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GameFramework/Character.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "PartBase.h"

#include "DataTypes.h"

#include "PartSelectorKiosk.generated.h"

class UWelderComponent;


UCLASS()
class GMTK2024_API ADisplayItemActor : public ACharacter {
	GENERATED_BODY()

public:

	

};


UCLASS()
class GMTK2024_API APartSelectorKiosk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APartSelectorKiosk();

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<APartBase>> parts;

	UPROPERTY(BlueprintReadWrite)
		int currentItem = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* currentDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* kioskMesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* testMesh;

	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* BoxCollider;


	UPROPERTY(BlueprintReadWrite)
		ADisplayItemActor* currentDisplayItem;
		

	UPROPERTY(BlueprintReadWrite)
		FVector holoLocation;

	UPROPERTY(EditAnywhere)
		UMaterial* HoloMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		PartType partType;

	UPROPERTY(BlueprintReadOnly)
		UWelderComponent* playerWelder;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		float localTime = 0.0f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
		void RotateDisplayItem(float value);

};
