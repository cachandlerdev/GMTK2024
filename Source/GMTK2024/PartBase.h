// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/SkeletalMeshComponent.h"

#include "PartBase.generated.h"

UCLASS(Blueprintable)
class GMTK2024_API APartBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APartBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* mesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
