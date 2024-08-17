// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"


#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputLibrary.h"

#include "UObject/UObjectGlobals.h"


#include "PartSelectorKiosk.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = jogSpeed;
	NumOfJumps = 2;

	TryRechargeSlideJumpBoost();
	DeferSetupMovementSystem();
}


void APlayerCharacter::DeferSetupMovementSystem()
{
	playerController = Cast<APlayerController>(GetLocalViewingPlayerController());


	//clear the copy mapping context and copy everything from the base defaults to the copy
	baseControlsCopy->UnmapAll();


	TArray<FEnhancedActionKeyMapping> mappings = baseControls->GetMappings();
	for (FEnhancedActionKeyMapping map : mappings)
	{
		FEnhancedActionKeyMapping* mapCopy = &baseControlsCopy->MapKey(map.Action, map.Key);

		mapCopy->Modifiers = map.Modifiers;


		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
		                                 map.Action.GetFName().ToString() + " | " + map.Key.GetFName().ToString());
	}
	UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(baseControlsCopy);


	if (playerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			playerController->GetLocalPlayer());

		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, playerController->GetName());
		FString name = playerController->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *name);

		if (Subsystem)
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(baseControlsCopy, baseControlsPriority);
		}


		movementSuccessful = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, GetName() + "Couldn't set up movement system.");
	}
}


// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	playerEnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerEnhancedInput)
	{
		if (movementAction && lookAction)
		{
			//BUILD ALL MAPPINGS THAT ARENT BUILT IN EDITOR

			//baseControlsCopy->MapKey(fireAction, EKeys::LeftMouseButton);
			//need to rebuild the control mapping
			//UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(baseControlsCopy);

			playerEnhancedInput->BindAction(movementAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::moveInput);
			//playerEnhancedInput->BindAction(airStrafeAction, ETriggerEvent::Started, this, &APlayerCharacter::airStrafeInput);
			//playerEnhancedInput->BindAction(forwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::forwardInput);
			playerEnhancedInput->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::lookInput);
			//playerEnhancedInput->BindAction(zoomAction, ETriggerEvent::Triggered, this, &APlayerCharacter::zoomInput);
			playerEnhancedInput->BindAction(sprintAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::ToggleSprint);
			playerEnhancedInput->BindAction(jumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::jumpInput);

			playerEnhancedInput->BindAction(crouchAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::crouchInput);

			playerEnhancedInput->BindAction(scrollAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::scrollInput);

			//playerEnhancedInput->BindAction(dodgeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::dodgeInput);
			//playerEnhancedInput->BindAction(ability1Action, ETriggerEvent::Triggered, this, &APlayerCharacter::ability1Input);
			//playerEnhancedInput->BindAction(targetLockAction, ETriggerEvent::Triggered, this, &APlayerCharacter::targetLockInput);
			//playerEnhancedInput->BindAction(fireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::fireInput);
			//playerEnhancedInput->BindAction(heavyFireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::heavyFireInput);
			//playerEnhancedInput->BindAction(aimAction, ETriggerEvent::Triggered, this, &APlayerCharacter::aimInput);
		}
	}
}


// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (sliding)
	{
		slideTimer += DeltaTime;
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, "sliding");
		if (GetCharacterMovement()->Velocity.Length() < 400.0f && sliding)
		{
			SetSliding(false);
		}
		else if (!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->Velocity *= (((CalcHillSlideBoost() - .9f) * DeltaTime * 10.0f) + 1.0f);
		}
	}
}


void APlayerCharacter::moveInput(const FInputActionValue& value)
{
	FVector2D passValue = value.Get<FVector2D>();
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, "Slide Timer: " + FString::SanitizeFloat(slideTimer));

	if (sliding)
	{
		if (slideTimer > .8f || !GetCharacterMovement()->IsFalling())
		{
			passValue.X = 0.0f;
			passValue.Y *= 0.01f;
			GetCharacterMovement()->AirControl = 0.05f;
		}
		else
		{
			GetCharacterMovement()->AirControl = 0.9f;
			passValue.X *= 1.05f;
			passValue.Y *= 1.1f;
		}
	}

	FVector straight = GetControlRotation().Vector();
	FVector right = GetControlRotation().Vector().Cross(-FVector::UpVector);
	FVector forward = right.Cross(FVector::UpVector);

	forward.Z = 0.0f;
	forward = forward.GetSafeNormal();

	right.Z = 0.0f;
	right = right.GetSafeNormal();

	AddMovementInput(forward, value.Get<FVector2D>().X * 10.0f);
	AddMovementInput(right, value.Get<FVector2D>().Y * 10.0f);

	if (sprinting && value.Get<FVector2D>().X < 0.5f)
	{
		SetSprinting(false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, FString::SanitizeFloat(value.Get<FVector2D>().X));
	}
}


void APlayerCharacter::lookInput(const FInputActionValue& value)
{
	AddControllerPitchInput(-1.0f * value.Get<FVector2D>().Y * lookSensitivity);
	AddControllerYawInput(value.Get<FVector2D>().X * lookSensitivity);
}


void APlayerCharacter::ToggleSprint(const FInputActionValue& value)
{
	// i want to have a release check on the input
	//(maybe for later, might change to only pressed tho) but I only want this to be sprinting if true
	if (value.Get<bool>())
	{
		SetSprinting(true);
		DoWhileSprinting();
	}
}

void APlayerCharacter::jumpInput(const FInputActionValue& value)
{
	if (NumOfJumps != 0)
	{
		NumOfJumps--;
		Jump();
		if (sliding && !GetCharacterMovement()->IsFalling())
		{
			//apply slide jump boost
			GetCharacterMovement()->Velocity *= (1.0f + slideJumpBoost);

			//max slide boost is 0.2 so after 2 jumps no more boost
			//and after 3 jumps they start loosing speed so they cant slide forever
			slideJumpBoost -= 0.1f;
		}
	}
}


void APlayerCharacter::TryRechargeSlideJumpBoost()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Orange, FString::SanitizeFloat(slideJumpBoost));

	//only charge if they are on the ground and are not sliding
	if (!GetCharacterMovement()->IsFalling() && !sliding)
	{
		//recharge up to the 20% slide boost
		if (slideJumpBoost < 0.2f)
		{
			//the slide jump boost can go below 0 if they do more than 2 jumps in a slide
			slideJumpBoost = fmaxf(0.0f, slideJumpBoost);

			slideJumpBoost += 0.1f;
		}
	}


	if (slideJumpRechargeTimerHandle.IsValid())
	{
		slideJumpRechargeTimerHandle.Invalidate();
	}

	GetWorldTimerManager().SetTimer(slideJumpRechargeTimerHandle, this, &APlayerCharacter::TryRechargeSlideJumpBoost,
	                                0.8f);
}


void APlayerCharacter::scrollInput(const FInputActionValue& value)
{
	FVector eyeLoc;
	FRotator eyeDir;

	GetActorEyesViewPoint(eyeLoc, eyeDir);

	TArray<FHitResult> hits;


	GetWorld()->LineTraceMultiByChannel(hits, GetActorLocation(), GetActorLocation() + (eyeDir.Vector() * 10000.0f),
	                                    ECC_Visibility);
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (eyeDir.Vector() * 10000.0f), 10.0f,
	                          FColor::Green, false, 1.0f);

	for (FHitResult hit : hits)
	{
		APartSelectorKiosk* kiosk = Cast<APartSelectorKiosk>(hit.GetActor());

		if (kiosk)
		{
			kiosk->RotateDisplayItem(value.Get<float>());
		}
	}
}


void APlayerCharacter::fireInput(const FInputActionValue& value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Base Fire Called");
}

void APlayerCharacter::heavyFireInput(const FInputActionValue& value)
{
}


void APlayerCharacter::crouchInput(const FInputActionValue& value)
{
	crouching = value.Get<bool>();

	if (value.Get<bool>())
	{
		StartCrouch();
		if (!GetCharacterMovement()->IsFalling())
		{
			SetSprinting(false);
		}
		if (GetCharacterMovement()->Velocity.Length() > 410.0f && !sliding)
		{
			SetSliding(true);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "NOT SLIDING");
			if (!GetCharacterMovement()->IsFalling())
			{
				GetCharacterMovement()->Velocity *= 1.3f;
			}
		}
	}

	if (!value.Get<bool>())
	{
		EndCrouch();
		SetSliding(false);
	}
}


void APlayerCharacter::dodgeInput(const FInputActionValue& value)
{
}

void APlayerCharacter::targetLockInput(const FInputActionValue& value)
{
}


void APlayerCharacter::aimInput(const FInputActionValue& value)
{
}

void APlayerCharacter::ability1Input(const FInputActionValue& value)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, "ability1 call");
}


void APlayerCharacter::SetSprinting(bool val)
{
	sprinting = val;
	if (sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = jogSpeed;
		DoWhenSprintingOver();
	}
}

bool APlayerCharacter::IsPlayerSprinting()
{
	return sprinting;
}


void APlayerCharacter::SetSliding(bool val)
{
	sliding = val;
	if (sliding)
	{
		slideTimer = 0.0f;

		GetCharacterMovement()->BrakingFriction = 0.3f;
		GetCharacterMovement()->GroundFriction = 0.3f;
	}
	else
	{
		GetCharacterMovement()->BrakingFriction = 8.0f;
		GetCharacterMovement()->GroundFriction = 8.0f;


		GetCharacterMovement()->AirControl = 0.05f;
	}
}


void APlayerCharacter::Landed(const FHitResult& hit)
{
	Super::Landed(hit);

	NumOfJumps = 2;
	EndWallRun(0.0f);
	bIsWallrunSuppressed = false;

	if (sliding || crouching)
	{
		FVector lateralVelocity = GetCharacterMovement()->Velocity;
		lateralVelocity.Z = 0.0f;
		
		//if not moving but pressing crouch after jump on a hill/bump, then give minimum slide speed
		if (lateralVelocity.Length() < 450.0f) {
			FVector forwardDir = GetActorForwardVector();
			forwardDir.Z = 0.0f;
			forwardDir = forwardDir.GetSafeNormal();

			GetCharacterMovement()->Velocity = forwardDir * 500.0f;
		}


		//change velocity based on the hill steepness and direction
		GetCharacterMovement()->Velocity *= (CalcHillSlideBoost() + 1.1f);
	}
}


float APlayerCharacter::CalcHillSlideBoost()
{
	float hillGrade = 0.0f;
	float hillOppose = 0.0f;

	TArray<FHitResult> hits;
	GetWorld()->LineTraceMultiByChannel(hits, GetActorLocation(), GetActorLocation() - FVector(0.0f, 0.0f, 500.0f),
	                                    ECC_Visibility);

	for (FHitResult hit : hits)
	{
		APlayerCharacter* tryCastPlayer = Cast<APlayerCharacter>(hit.GetActor());

		if (!IsValid(tryCastPlayer))
		{
			//how steep is this hill
			hillGrade = 1.0f - FMath::Abs(hit.ImpactNormal.Dot(FVector(0.0f, 0.0f, 1.0f)));

			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::SanitizeFloat(hillGrade));

			//cap the boost at 1.5f * velocity
			//hillGrade *= 0.5f;

			// ** get whether or not we're moving into the hill or with the hill

			//get hill xy direction 
			FVector hillDir = hit.ImpactNormal;
			hillDir.Z = 0.0f;
			hillDir = hillDir.GetSafeNormal();

			//get velocity direction in xy play normalized
			FVector vDir = GetCharacterMovement()->Velocity;
			vDir.Z = 0.0f;
			vDir = vDir.GetSafeNormal();

			//calculate whether the hill opposes players movement direction
			hillOppose = FVector::DotProduct(hillDir, vDir);

			// **


			//only do this for the first hit below the player
			break;
		}
	}
	return (1.0f + (hillOppose * hillGrade));
}


void APlayerCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}
