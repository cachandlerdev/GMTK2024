// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DataTypes.h"
#include "TicketBoardActor.h"

#include "TicketActor.generated.h"

class AMyGameMode;
class AChasisPartBase;

USTRUCT(BlueprintType)
struct GMTK2024_API FOrder
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		FString companyName = "";

	UPROPERTY(BlueprintReadWrite)
		float cost = 200.0f;

	UPROPERTY(BlueprintReadWrite)
		float structural = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float firepower = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float thrust = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float energy = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float support = 100.0f;


	UPROPERTY(BlueprintReadWrite)
		float shield = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float powerSurplus = 10.0f;


	UPROPERTY(BlueprintReadWrite)
		ShipType shipType = ShipType::ST_SMALL;

	UPROPERTY(BlueprintReadWrite)
		float accuracy = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		float customerPatience = 30.0f;

};



UCLASS()
class GMTK2024_API ATicketActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATicketActor();

	UPROPERTY(BlueprintReadOnly)
		float ticketTime = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		FOrder order;

	UPROPERTY(BlueprintReadWrite)
		FOrder progressOrder;


	UPROPERTY(BlueprintReadWrite)
		AChasisPartBase* shipChassis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATicketBoardActor* TicketBoard;


	UPROPERTY()
		AMyGameMode* gameMode;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	


};
