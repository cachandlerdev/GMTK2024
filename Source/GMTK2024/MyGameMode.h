// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "DataTypes.h"

#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2024_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AMyGameMode(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ChasisActorType;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float difficulty = 1.0f;



	UPROPERTY(BlueprintReadWrite)
		UOrderDataSheet* orderSheet;




	UPROPERTY(BlueprintReadWrite)
		AActor* currentShip;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	UFUNCTION(BlueprintCallable)
		void AddOrder();


	UFUNCTION(BlueprintCallable)
		void SpawnShipChasis();

	UFUNCTION(BlueprintImplementableEvent)
		void SpawnShipChasisBP();

};
