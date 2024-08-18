// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DataTypes.generated.h"

#define NUM_PART_TYPES 5


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


UENUM(BlueprintType)
enum class PartType : uint8 {
	PT_STRUCTURAL = 0 UMETA(DisplayName = "STRUCTURAL"),
	PT_FIREPOWER = 1  UMETA(DisplayName = "FIREPOWER"),
	PT_THRUST = 2     UMETA(DisplayName = "TRUST"),
	PT_ENERGY = 3     UMETA(DisplayName = "ENERGY"),
	PT_MISC = 4		UMETA(DisplayName = "MISC")
};

/*
PartType operator++(PartType& left) {


	switch (left) {

	case PartType::PT_STRUCTURAL:
		return PartType::PT_FIREPOWER;
	case PartType::PT_FIREPOWER:
		return PartType::PT_THRUST;
	case PartType::PT_THRUST:
		return PartType::PT_ENERGY;
	case PartType::PT_ENERGY:
		return PartType::PT_MISC;
	case PartType::PT_MISC:
		return PartType::PT_STRUCTURAL;

	}


}

PartType operator--(PartType& left) {


	switch (left) {

	case PartType::PT_STRUCTURAL:
		return PartType::PT_MISC;
	case PartType::PT_FIREPOWER:
		return PartType::PT_STRUCTURAL;
	case PartType::PT_THRUST:
		return PartType::PT_FIREPOWER;
	case PartType::PT_ENERGY:
		return PartType::PT_THRUST;
	case PartType::PT_MISC:
		return PartType::PT_ENERGY;

	}


}
*/



/**
 * 
 */
class GMTK2024_API DataTypes
{
public:
	DataTypes();

	

	~DataTypes();
};
