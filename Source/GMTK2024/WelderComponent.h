// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "DataTypes.h"

#include "Animation/SkeletalMeshActor.h"

#include "WelderComponent.generated.h"


class APartBase;
class APlayerCharacter;



/**
 * 
 */
UCLASS()
class GMTK2024_API UWelderComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UWelderComponent(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(BlueprintReadOnly)
		APlayerCharacter* OwningPlayer;




	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<APartBase> structuralPart;

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<APartBase> firepowerPart;

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<APartBase> energyPart;

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<APartBase> thrustPart;

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<APartBase> miscPart;

	
	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<APartBase> equippedPart;




	UPROPERTY(BlueprintReadWrite)
		APartBase* inProgressWeld;


	UPROPERTY(BlueprintReadOnly)
		ASkeletalMeshActor* blueprintActor;

	UPROPERTY(BlueprintReadOnly)
		UMaterial* HoloMaterial;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	UFUNCTION(BlueprintCallable)
		void WeldInput();

	UFUNCTION(BlueprintCallable)
		void BlueprintInput();






	UFUNCTION(BlueprintCallable)
		void SetEquippedPart(PartType type);

	UFUNCTION(BlueprintCallable)
		void SetPartTypePart(PartType type, TSubclassOf<APartBase> part);
};
