// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"


#include "GameplayTags.h"

#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"


#include "PlayerCharacter.generated.h"

class UEnhancedInputComponent;
class UInputAction;
class UInputMappingContext;
class USoundBase;
class USoundCue;

class UWelderComponent;
class AMyGameMode;


class ATicketActor;

UCLASS()
class GMTK2024_API APlayerCharacter : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	// The welder object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWelderComponent* Welder;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UWelderComponent> welderClass;

	UPROPERTY(BlueprintReadOnly)
	AMyGameMode* gameMode;

	UPROPERTY(BlueprintReadOnly)
	APlayerController* playerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags")
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	float lookSensitivity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float sprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float jogSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float slideJumpBoost = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WallRunSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWallrunHasGravity = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WallrunTargetGravity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WallRunJumpHeight = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WallRunJumpOffForce = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float WallRunCameraTilt = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SprintFovIncrease = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashStrength = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCooldown = 5.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* FootstepSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* WallrunSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* LandingSound;


	UPROPERTY(BLueprintReadWrite)
		ATicketActor* currentlyHeldTicket;


private:
	bool sprinting = false;
	bool sliding = false;
	bool crouching = false;
	float slideTimer = 0.0f;
	FTimerHandle slideJumpRechargeTimerHandle;
	int NumOfJumps;
	int DefaultGravityScale;

	FVector DefaultMaxVelocity;

	// Used to handle the wall run update
	FTimerHandle WallrunTimerHandle;
	// How frequently we update the wallrun status
	float WallrunUpdateTime = 0.02f;

	// True if the player is wall running on the right wall
	bool bIsWallRunningRight = false;
	// True if the player is wall running on the left wall
	bool bIsWallRunningLeft = false;

	// True if the wallrun is suppressed.
	bool bIsWallrunSuppressed = false;
	// Used to un-suppress the wall run
	FTimerHandle ResetSuppressWallrunTimerHandle;

	// The normal vector of the wall we're running on.
	FVector WallRunNormal;

	// Used to track the initial field of view.
	float InitialFov;

	// Used to make sure the FOV doesn't go too high when sprinting
	float MaxFov;

	// Whether the player is dashing
	bool bIsOnDashCooldown = false;

	// used to track the dash cooldown reset.
	FTimerHandle DashCooldownHandle;

	// Keeps track of whether the player can mantle
	bool bCanMantle = true;

	// How often we run the footsteps check
	float FootstepsUpdateTime = 0.3f;

public:
	//INPUT STUFF
	UPROPERTY(BlueprintReadOnly)
	UEnhancedInputComponent* playerEnhancedInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* movementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* lookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* jumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* sprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* fireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* scrollAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* crouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* menuAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* aimAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* DashAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
	UInputAction* InteractAction;

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

	virtual void Landed(const FHitResult& hit) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//needed for gameplay tags
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	//setup movement after begin so that player controller never fails to acquire input
	UFUNCTION(BlueprintCallable)
	void DeferSetupMovementSystem();

	// Makes the player slide
	UFUNCTION()
	void SetSliding(bool val);

	// Makes the player sprint
	UFUNCTION()
	void SetSprinting(bool val);

	// Returns whether the player is sprinting
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPlayerSprinting();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCrouch();

	UFUNCTION(BlueprintImplementableEvent)
	void EndCrouch();

	// Returns whether the player is wall running.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsWallRunning();

	// Returns whether the player is on a dash cooldown
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsOnDashCooldown();

	// Runs when the player dashes.
	UFUNCTION(BlueprintImplementableEvent)
	void OnDash();

	// Runs when the player mantles.
	UFUNCTION(BlueprintImplementableEvent)
	void OnMantle();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDashCooldownTimeRemaining();

	// Inputs

	UFUNCTION()
	virtual void lookInput(const FInputActionValue& value);

	UFUNCTION()
	virtual void fireInput(const FInputActionValue& value);

	UFUNCTION()
	virtual void moveInput(const FInputActionValue& value);

	UFUNCTION()
	virtual void ToggleSprint(const FInputActionValue& value);

	UFUNCTION()
	virtual void aimInput(const FInputActionValue& value);

	UFUNCTION()
	virtual void scrollInput(const FInputActionValue& value);

	UFUNCTION()
	virtual void crouchInput(const FInputActionValue& value);

	UFUNCTION()
	virtual void jumpInput(const FInputActionValue& value);

	// A blueprint implementable event that can be run when the player crouches. Optional
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoWhileCrouching(bool crouchVal);

	// A blueprint implementable event that can be run when the player sprints.
	UFUNCTION(BlueprintImplementableEvent)
	void DoWhileSprinting();

	// A blueprint implementable event that runs after the player has stopped sprinting.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoWhenSprintingOver();

	// Plays when the player jumps.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnJump(bool bIsAirJump);

	UFUNCTION(BlueprintCallable)
	void Dash();

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION()
	float CalcHillSlideBoost();

	UFUNCTION()
	void TryRechargeSlideJumpBoost();

	UFUNCTION()
	void WelderAttachmentCallback();

private:
	// Camera

	// Updates the player's FOV depending on his movement speed.
	void UpdateFovTick(float DeltaTime);

	// Wallrun

	// Runs repeatedly to update wall running
	void WallRunUpdate();

	// Performs a line trace and launches the character to help him stick to the wall if need be.
	// Returns true if he is on the wall
	bool WallRunMovement(FVector Start, FVector End, float WallRunDirection);

	// Resets the wallrun status and the player's gravity.
	void EndWallRun(float ResetTime);

	// Suppresses the wallrun for a specified amount of time.
	void SuppressWallRun(float Delay);

	// Disables wall run suppression.
	void ResetWallRunSuppression();

	// Tilts the camera depending on whether we're wallrunning left/right
	void CameraTick();

	// Tilt the camera a specific direction.
	void TiltCamera(float Roll);

	// Lets the player jump off of a wallrun.
	void WallRunJump();

	// Dash

	// Perform the actual dash
	void PerformDash();

	// Allows the player to dash again
	void SetDashCooldownOver();

	// Mantling

	// Performs a line trace to see if enough of the player's body is high enough to mantle
	void MantlingTick();

	// Performs a mantle.
	void Mantle();

	// Used to determine whether the player has gone far enough to play a footstep.
	void CheckFootsteps();
	
};
