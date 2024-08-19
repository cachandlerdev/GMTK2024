// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TicketBoardActor.generated.h"

class ATicketActor;
class AChasisPartBase;

UCLASS()
class GMTK2024_API ATicketBoardActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATicketBoardActor();

	//can the player click this board to unplug the ticket
	UPROPERTY(EditAnywhere)
		bool unpluggable = false;

	UPROPERTY(EditAnywhere)
		AActor* shipBayActor;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AChasisPartBase> chassisClass;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		ATicketActor* pluggedTicket;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
		void PlugTicketIn(ATicketActor* ticket);

	UFUNCTION(BlueprintImplementableEvent)
		void PlugTicketInBP();

	UFUNCTION(BlueprintCallable)
		ATicketActor* UnplugTicket();

	UFUNCTION(BlueprintCallable)
		ATicketActor* GetPluggedTicket();

};