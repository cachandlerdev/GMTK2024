// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "DataTypes.h"


#include "Sound/SoundCue.h"

#include "Sound/SoundBase.h"

#include "MyGameMode.generated.h"

class APartBase;
class USoundCue;
class USoundBase;

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


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UOrderDataSheet* orderSheet;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* ShiftStartAlarm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* ShiftStartSoundBase;


	UPROPERTY()
	FTimerHandle shiftStatTimerHandle;

	UPROPERTY()
	FTimerHandle addOrderTimerHandle;


	UPROPERTY(BlueprintReadOnly)
	bool shipUnderConstruction = false;

	UPROPERTY(BlueprintReadOnly)
	APartBase* currentShipChassis;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	FVector newShipStartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	FVector shipConstructionLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	FVector shipLaunchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	float transitionTime = 2.0f;


	UPROPERTY()
	FTimerHandle moveTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	float localTime = 0.0f;

public:
	virtual void Tick(float deltaTime) override;


	UFUNCTION(BlueprintCallable)
	void AddOrder();


	UFUNCTION(BlueprintCallable)
	void SpawnShipChasis();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnShipChasisBP();


	UFUNCTION()
	void MoveShipToLocationOverTime(FVector startLocation, FVector endLocation, float overTime, int nCalls);


	UFUNCTION()
	void ShiftStartCallback();


	UFUNCTION()
	void DoNewShipChassisProcedure();

	UFUNCTION()
	void DoFinishOrderProcedure();

	//UFUNCTION()
	//void AddOrderCallback();
};
