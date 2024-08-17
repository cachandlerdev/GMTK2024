// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "PartBase.h"

#include "PartSelectorKiosk.generated.h"



UCLASS()
class GMTK2024_API ADisplayItemActor : public AActor {
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

	UPROPERTY(BlueprintReadWrite)
		USkeletalMeshComponent* currentDisplay;

	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* BoxCollider;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
		void RotateDisplayItem(float value);

};
