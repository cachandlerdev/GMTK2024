// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "DataTypes.h"


#include "Sound/SoundCue.h"


#include "Sound/SoundBase.h"

#include "MyGameMode.generated.h"


class AChasisPartBase;

class ATicketActor;
class ATicketBoardActor;

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


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float difficulty = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> companyNames;


	UPROPERTY(EditAnywhere)
	TSubclassOf<ATicketActor> ticketSpawnClass;

	UPROPERTY(BlueprintReadWrite)
	TArray<ATicketActor*> tickets;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	float localTime = 0.0f;

private:
	bool bIsOnShift = false;

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
	void AddPartToBuild(APartBase* part, ATicketActor* ticket);

	UFUNCTION(BlueprintImplementableEvent)
	void PartAddedBP(ATicketActor* Ticket);


	UFUNCTION(BlueprintCallable)
	void DoShipFlight(ATicketActor* ticket);

	UFUNCTION()
	void DoNewShipChassisProcedure(TSubclassOf<AChasisPartBase> chassisType, ATicketActor* ticket,
	                               AActor* bayTransformActor);

	UFUNCTION()
	void DoFinishOrderProcedure(ATicketActor* ticket);

	UFUNCTION()
	void CompleteGradingAfterFlight(ATicketActor* ticket);


	UFUNCTION()
	void CleanupShip(ATicketActor* ticket);


	UFUNCTION()
	void EndShiftProcedure();


	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
	FReportCard EvaluateBuildWithOrder(ATicketActor* ticket);

	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
	float GetHarmonyGrade(ATicketActor* ticket);

	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
	FVector GetShipCenterOfMass(ATicketActor* ticket);

	//finds the ship in the world and compares it to the given order
	UFUNCTION(BlueprintCallable)
	void GetShipThrust(FVector& centerOfThrust, FVector& thrustVector, ATicketActor* ticket);


	UFUNCTION(BlueprintImplementableEvent)
	void UpdateQuotaBP();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOnShift();
};
