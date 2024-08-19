// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DataTypes.generated.h"

#define NUM_PART_TYPES 5


USTRUCT(BlueprintType)
struct GMTK2024_API FOrder
{
	GENERATED_BODY()

public:

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
		float accuracy = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		float customerPatience = 30.0f;

};


USTRUCT(BlueprintType)
struct GMTK2024_API FReportCard
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	float overall = 0.0f;


	UPROPERTY(BlueprintReadWrite)
		float cost = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float structural = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float firepower = 0.0f;

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
	float harmony = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float fractionOfRunwayCovered = 0.0f;


	UPROPERTY(BlueprintReadWrite)
		float customerPatience = 30.0f;


	
};


UCLASS(Blueprintable)
class GMTK2024_API UOrderDataSheet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FOrder> orders;

	UPROPERTY(BlueprintReadWrite)
		FOrder currentOrder;

	UPROPERTY(BlueprintReadWrite)
		FOrder currentBuildOrder;

	UPROPERTY(BlueprintReadWrite)
	TArray<FReportCard> reports;

	UPROPERTY(BlueprintReadWrite)
	TArray<float> grades;

	UPROPERTY(BlueprintReadWrite)
	float averageGrade;
};


UENUM(BlueprintType)
enum class PartType : uint8
{
	PT_STRUCTURAL = 0 UMETA(DisplayName = "STRUCTURAL"),
	PT_FIREPOWER = 1 UMETA(DisplayName = "FIREPOWER"),
	PT_THRUST = 2 UMETA(DisplayName = "THRUST"),
	PT_ENERGY = 3 UMETA(DisplayName = "ENERGY"),
	//PT_MISC = 4 UMETA(DisplayName = "MISC")
	PT_SUPPORT = 4		UMETA(DisplayName = "SUPPORT"),
	PT_CHASSIS = 5		UMETA(DisplayName = "CHASSIS")
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
