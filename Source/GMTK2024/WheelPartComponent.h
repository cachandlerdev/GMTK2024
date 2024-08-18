// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartComponent_Base.h"



#include "WheelPartComponent.generated.h"


class AMyGameMode;

/**
 * 
 */
UCLASS()
class GMTK2024_API UWheelPartComponent : public UPartComponent_Base
{
	GENERATED_BODY()

public:

	UWheelPartComponent(const FObjectInitializer& ObjectInitializer);
	
	



	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
