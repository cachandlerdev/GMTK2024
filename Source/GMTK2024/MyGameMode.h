// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "DataTypes.h"


#include "Sound/SoundCue.h"


#include "Sound/SoundBase.h"

#include "MyGameMode.generated.h"


class AChasisPartBase;

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

	UPROPERTY(BlueprintReadWrite)
		int shiftQuota = 1;


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
	int nShipsUnderConstruction = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<AChasisPartBase*> currentShipChassis;


	UPROPERTY(BlueprintReadWrite)
		float buildTime = 0.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxFlightTime = 5.0f;


	UPROPERTY(EditAnywhere)
		float baseAngularDamping = 50.0f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	FVector newShipStartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	FVector shipConstructionLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	FVector shipLaunchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Pathing")
	float transitionTime = 2.0f;



	UPROPERTY()
		FReportCard reportInProgress;


	UPROPERTY()
	FTimerHandle moveTimerHandle;

	UPROPERTY()
		FTimerHandle finsihGradingHandle;


	UPROPERTY()
		bool finishingOrder = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	float localTime = 0.0f;

public:
	virtual void Tick(float deltaTime) override;


	UFUNCTION(BlueprintCallable)
	void AddOrder();

	UFUNCTION(BlueprintImplementableEvent)
		void AddOrderBP();


	

	// A blueprint event that gets called when the player's shift ends.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnShiftEnd();

	UFUNCTION(BlueprintCallable)
		FOrder GetZeroOrder();


	UFUNCTION()
	void MoveShipToLocationOverTime(FVector startLocation, FVector endLocation, float overTime, int nCalls);


	UFUNCTION()
	void ShiftStartCallback();





	UFUNCTION(BlueprintCallable)
		void AddPartToBuildOrder(APartBase* part);

	UFUNCTION(BlueprintImplementableEvent)
		void PartAddedBP();


	

	UFUNCTION(BlueprintCallable)
		void DoShipFlight(int shipID);

	UFUNCTION()
	void DoNewShipChassisProcedure(int shipID);

	UFUNCTION()
	void DoFinishOrderProcedure(int shipID, int ticketID);

	UFUNCTION()
		void CompleteGradingAfterFlight(int shipID);




	UFUNCTION()
		void CleanupShip(int shipID);



	UFUNCTION()
		void EndShiftProcedure();


	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
		FReportCard EvaluateBuildWithOrder(FOrder order, int shipID);

	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
		float GetHarmonyGrade(int shipID);

	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
		FVector GetShipCenterOfMass(int shipID);

	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
		void GetShipThrust(FVector& centerOfThrust, FVector& thrustVector, int shipID);



	UFUNCTION(BlueprintImplementableEvent)
		void UpdateQuotaBP();
	
};
