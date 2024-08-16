// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameplayTags.h"

#include "InputAction.h"

#include "PlayerCharacter.generated.h"


class UInputAction;
class UInputMappingContext;
class USoundBase;
class USoundCue;


UCLASS()
class GMTK2024_API APlayerCharacter : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(BlueprintReadOnly)
		APlayerController* playerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags")
		FGameplayTagContainer GameplayTags;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		float lookSensitivity = 0.5f;





	//INPUT STUFF
	UPROPERTY(BlueprintReadOnly)
		UEnhancedInputComponent* playerEnhancedInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* movementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* airStrafeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* lookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* jumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* sprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* fireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* heavyFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* zoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* forwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* crouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* dodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* menuAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* swapWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* aimAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* ability1Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
		UInputAction* targetLockAction;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Mappings")
		UInputMappingContext* baseControls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Mappings")
		UInputMappingContext* baseControlsCopy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Mappings")
		int32 baseControlsPriority = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




	UPROPERTY(BlueprintReadOnly)
		bool movementSuccessful = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//needed for gameplay tags
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;



	//setup movement after begin so that player controller never fails to acquire input
	UFUNCTION(BlueprintCallable)
		void DeferSetupMovementSystem();


	UFUNCTION()
		virtual void lookInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void fireInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void heavyFireInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void moveInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void sprintInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void aimInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void crouchInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void dodgeInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void jumpInput(const FInputActionValue& value);

	UFUNCTION()
		virtual void ability1Input(const FInputActionValue& value);

	UFUNCTION()
		virtual void targetLockInput(const FInputActionValue& value);

};
