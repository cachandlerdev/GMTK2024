// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DataTypes.generated.h"


USTRUCT(BlueprintType)
struct GMTK2024_API FOrder {

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		float firepower = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float shield = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float thrust = 100.0f;

	UPROPERTY(BlueprintReadWrite)
		float powerSurplus = 10.0f;

	

	UPROPERTY(BlueprintReadWrite)
		int numEMWeapons = 0;

	UPROPERTY(BlueprintReadWrite)
		int numKineticWeapons = 0;

	

};


UCLASS(Blueprintable)
class GMTK2024_API UOrderDataSheet : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		TArray<FOrder> orders;


};





/**
 * 
 */
class GMTK2024_API DataTypes
{
public:
	DataTypes();
	~DataTypes();
};
