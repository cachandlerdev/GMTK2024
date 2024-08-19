// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

#include "DataTypes.h"



#include "PartBase.generated.h"

UCLASS(Blueprintable)
class GMTK2024_API APartBase : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APartBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PartName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* physicsBox;

	UPROPERTY(BlueprintReadOnly)
		APartBase* weldTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float weldTimeRequirement;

	UPROPERTY(BlueprintReadWrite)
		float weldTime;

	UPROPERTY(BlueprintReadOnly)
		float localTimeAtLastWeldCheckin;


	//the attribute of the part abstracted away from the part type
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float baseAttribute = 25.0f;

	//the cost of the part
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float cost = 25.0f;


	//the mass of the part
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float mass = 25.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		PartType type;


	
	UPROPERTY(BlueprintReadOnly)
		int shipID = -1;



	UPROPERTY(BlueprintReadWrite)
		bool launched = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
		float localTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//returns true if weld has completed, false if weld is still incomplete.
	UFUNCTION(BlueprintCallable)
		bool ProgressWeld();

	UFUNCTION(BlueprintCallable)
		void SolidifyWeld();

	UFUNCTION(BlueprintCallable)
		virtual void ActivatePart();

	UFUNCTION(BlueprintImplementableEvent)
		void ActivatePartBP();

};
