// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"


#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputLibrary.h"
#include "KismetTraceUtils.h"


#include "Kismet/KismetMathLibrary.h"

#include "UObject/UObjectGlobals.h"


#include "PartSelectorKiosk.h"

#include "GameFramework/CharacterMovementComponent.h"


#include "WelderComponent.h"

#include "SpawnChasisActor.h"
#include "FinishOrderActor.h"


#include "Kismet/GameplayStatics.h"
#include "MyGameMode.h"

#include "TicketActor.h"
#include "TicketBoardActor.h"


// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	// Add welder
	//Welder = CreateDefaultSubobject<USkeletalMeshComponent>("Welder");

	FVector welderLocation = FVector(40.0f, 30.0f, -30.0f);
	//Welder->AddLocalOffset(welderLocation, false);

	FRotator rotation = FRotator(0.0f, 180.0f, 0.0f);
	//Welder->AddLocalRotation(rotation);

	FVector scale = FVector(0.3f, 0.3f, 0.3f);
	//Welder->SetWorldScale3D(scale);
	//Welder->SetCollisionProfileName(TEXT("OverlapAll"));

	// setup constants
	InitialFov = Camera->FieldOfView;
	MaxFov = Camera->FieldOfView + SprintFovIncrease;

	GetCharacterMovement()->AirControl = 0.25f;
	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	// Attach things
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	Camera->AttachToComponent(GetCapsuleComponent(), Rules);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	Camera->bUsePawnControlRotation = true;


	GetMesh()->AttachToComponent(Camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetVisibility(false, false);


	Welder = Cast<UWelderComponent>(AddComponentByClass(welderClass, true, GetActorTransform(), false));


	if (Welder)
	{
		Welder->OwningPlayer = this;

		Welder->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "handSocket");
	}
	else
	{
		FTimerHandle tempHandle;

		GetWorld()->GetTimerManager().SetTimer(tempHandle, this, &APlayerCharacter::WelderAttachmentCallback, 0.1f);
	}


	GetCharacterMovement()->MaxWalkSpeed = jogSpeed;
	NumOfJumps = 2;
	DefaultGravityScale = GetCharacterMovement()->GravityScale;

	TryRechargeSlideJumpBoost();
	DeferSetupMovementSystem();

	GetWorld()->GetTimerManager().SetTimer(WallrunTimerHandle, this, &APlayerCharacter::WallRunUpdate,
	                                       WallrunUpdateTime, true);


	// Setup footsteps loop
	FTimerHandle footstepsHandle;
	GetWorld()->GetTimerManager().SetTimer(footstepsHandle, this, &APlayerCharacter::CheckFootsteps,
	                                       FootstepsUpdateTime, true);

	currentlyHeldTicket = nullptr;

}


void APlayerCharacter::WelderAttachmentCallback()
{
	if (Welder)
	{
		Welder->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "handSocket");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Orange, "Welder was never created.");
	}
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
	}
	UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(baseControlsCopy);


	if (playerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			playerController->GetLocalPlayer());

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
			playerEnhancedInput->BindAction(movementAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::moveInput);
			playerEnhancedInput->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::lookInput);
			playerEnhancedInput->BindAction(sprintAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::ToggleSprint);
			playerEnhancedInput->BindAction(jumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::jumpInput);

			playerEnhancedInput->BindAction(crouchAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::crouchInput);

			playerEnhancedInput->BindAction(scrollAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::scrollInput);
			playerEnhancedInput->BindAction(DashAction, ETriggerEvent::Triggered, this,
			                                &APlayerCharacter::Dash);

			playerEnhancedInput->BindAction(fireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::fireInput);
			playerEnhancedInput->BindAction(aimAction, ETriggerEvent::Triggered, this, &APlayerCharacter::aimInput);
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
		if (GetCharacterMovement()->Velocity.Length() < 400.0f && sliding)
		{
			SetSliding(false);
		}
		else if (!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->Velocity *= (((CalcHillSlideBoost() - .9f) * DeltaTime * 10.0f) + 1.0f);
		}
	}
	CameraTick();
	UpdateFovTick(DeltaTime);
	MantlingTick();
}


void APlayerCharacter::moveInput(const FInputActionValue& value)
{
	FVector2D passValue = value.Get<FVector2D>();

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
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, FString::SanitizeFloat(value.Get<FVector2D>().X));
	}
}


bool APlayerCharacter::IsWallRunning()
{
	return bIsWallRunningLeft || bIsWallRunningRight;
}

bool APlayerCharacter::IsOnDashCooldown()
{
	return bIsOnDashCooldown;
}

float APlayerCharacter::GetDashCooldownTimeRemaining()
{

	return GetWorld()->GetTimerManager().GetTimerRemaining(DashCooldownHandle);

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
	bool isMoving = GetVelocity().Length() > 10.0f;
	if (value.Get<bool>() && isMoving)
	{
		SetSprinting(true);
		DoWhileSprinting();
	}
}

void APlayerCharacter::jumpInput(const FInputActionValue& value)
{
	if (bCanMantle)
	{
		Mantle();
	}
	if (NumOfJumps > 0)
	{
		if (IsWallRunning())
		{
			WallRunJump();
		}
		else
		{
			OnJump(GetCharacterMovement()->IsFalling());
			Jump();
			NumOfJumps--;
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
}


void APlayerCharacter::TryRechargeSlideJumpBoost()
{
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

void APlayerCharacter::UpdateFovTick(float DeltaTime)
{
	if (Camera == nullptr)
	{
		return;
	}
	float threshhold = jogSpeed + 10.0f;
	float currentSpeed = GetVelocity().Length();
	bool bShouldIncreaseFov = currentSpeed > threshhold;

	if (bShouldIncreaseFov)
	{
		float currentFov = Camera->FieldOfView;
		float targetFov = Camera->FieldOfView + SprintFovIncrease;
		float newFov = UKismetMathLibrary::Lerp(currentFov, targetFov, DeltaTime);
		float clampedFov = UKismetMathLibrary::FClamp(newFov, 0.0f, MaxFov);
		Camera->SetFieldOfView(clampedFov);
	}
	else
	{
		float currentFov = Camera->FieldOfView;
		float newFov = UKismetMathLibrary::Lerp(currentFov, InitialFov, DeltaTime * 5.0f);
		Camera->SetFieldOfView(newFov);
	}
}

void APlayerCharacter::WallRunUpdate()
{
	if (bIsWallrunSuppressed)
	{
		// Ignore if the wallrun is suppressed.
		return;
	}

	// Get right end point
	FVector location = GetActorLocation();
	FVector rightVector = GetActorRightVector();
	FVector forwardVector = GetActorForwardVector();
	FVector rightEndPoint = location + (rightVector * 75.0f) + (forwardVector * -35.0f);
	FVector leftEndPoint = location + (rightVector * -75.0f) + (forwardVector * -35.0f);

	bool bShouldDropGravity = false;

	// Run the right wall run movement check + wall stick + boost
	if (WallRunMovement(location, rightEndPoint, -1.0f))
	{
		// Is running on right wall
		bIsWallRunningRight = true;
		bIsWallRunningLeft = false;
		bShouldDropGravity = true;
	}
	else if (bIsWallRunningRight)
	{
		// We're ending the right wall run
		EndWallRun(1.0f);
	}
	else if (WallRunMovement(location, leftEndPoint, 1.0f))
	{
		// Is running on left wall
		bIsWallRunningLeft = true;
		bIsWallRunningRight = false;
		bShouldDropGravity = true;
	}
	else if (bIsWallRunningLeft)
	{
		EndWallRun(1.0f);
	}

	if (bShouldDropGravity)
	{
		// Drop gravity
		float currentGravity = GetCharacterMovement()->GravityScale;
		float deltaTime = GetWorld()->GetDeltaSeconds();
		float interpSpeed = 10.0f;
		float newGravityScale = FMath::FInterpTo(currentGravity, WallrunTargetGravity, deltaTime, interpSpeed);
		GetCharacterMovement()->GravityScale = newGravityScale;
	}
}

bool APlayerCharacter::WallRunMovement(FVector Start, FVector End, float WallRunDirection)
{
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannelProperty, QueryParams);

	bool bIsValidWallVector = (Hit.Normal.Z > -0.52f && Hit.Normal.Z < 0.52f);
	if (Hit.bBlockingHit && bIsValidWallVector && GetCharacterMovement()->IsFalling())
	{
		WallRunNormal = Hit.Normal;

		// Stick the character to the wall
		float length = (Hit.Normal - GetActorLocation()).Length();
		FVector wallLaunchVelocity = Hit.Normal * length;
		LaunchCharacter(wallLaunchVelocity, false, false);

		// Move the character forward
		FVector upVector = FVector(0.0f, 0.0f, 1.0f);
		FVector forwardVector = UKismetMathLibrary::Cross_VectorVector(Hit.Normal, upVector);
		float multiplier = WallRunSpeed * WallRunDirection;
		FVector forwardLaunchVelocity = forwardVector * multiplier;
		LaunchCharacter(forwardLaunchVelocity, true, !bWallrunHasGravity);

		// Restore the two jumps
		NumOfJumps = 2;
		this->JumpCurrentCount = 0;

		// Return true because he's on the wall
		return true;
	}
	// He's not on a wall
	return false;
}

void APlayerCharacter::EndWallRun(float ResetTime)
{
	if (IsWallRunning())
	{
		bIsWallRunningLeft = false;
		bIsWallRunningRight = false;
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
		SuppressWallRun(ResetTime);
	}
}

void APlayerCharacter::SuppressWallRun(float Delay)
{
	bIsWallrunSuppressed = true;
	GetWorld()->GetTimerManager().SetTimer(ResetSuppressWallrunTimerHandle, this,
	                                       &APlayerCharacter::ResetWallRunSuppression, Delay, false);
}

void APlayerCharacter::ResetWallRunSuppression()
{
	GetWorld()->GetTimerManager().ClearTimer(ResetSuppressWallrunTimerHandle);
	bIsWallrunSuppressed = false;
}

void APlayerCharacter::CameraTick()
{
	if (bIsWallRunningLeft)
	{
		TiltCamera(WallRunCameraTilt);
	}
	else if (bIsWallRunningRight)
	{
		TiltCamera(-1 * WallRunCameraTilt);
	}
	else
	{
		TiltCamera(0.0f);
	}
}

void APlayerCharacter::TiltCamera(float Roll)
{
	FRotator current = GetControlRotation();

	float pitch = GetControlRotation().Pitch;
	float yaw = GetControlRotation().Yaw;
	FRotator target = FRotator(pitch, yaw, Roll);

	float deltaTime = GetWorld()->GetDeltaSeconds();
	FRotator newRotation = FMath::RInterpTo(current, target, deltaTime, 10.0f);

	GetController()->SetControlRotation(newRotation);
}

void APlayerCharacter::WallRunJump()
{
	if (IsWallRunning())
	{
		NumOfJumps--;
		OnJump(true);
		EndWallRun(0.25f);
		float xVelocity = WallRunNormal.X * WallRunJumpOffForce;
		float yVelocity = WallRunNormal.Y * WallRunJumpOffForce;
		FVector launchVelocity = FVector(xVelocity, yVelocity, WallRunJumpHeight);
		LaunchCharacter(launchVelocity, false, true);
	}
}

void APlayerCharacter::PerformDash()
{
	OnDash();

	FHitResult Hit;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = GetActorLocation() + FVector(0.0f, 0.0f, -1000.0f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);

	FVector launchVelocity = GetActorForwardVector();
	bool isInAir = (!Hit.bBlockingHit || Hit.Distance > 200.0f);
	if (isInAir)
	{
		FString distance = FString::SanitizeFloat(Hit.Distance);

		float smallifier = 0.01f;

		float newSpeed = GetVelocity().Length() + (DashStrength * smallifier);
		if (newSpeed > sprintSpeed)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Orange, "Clamp boost");
			launchVelocity *= DashStrength * smallifier;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Orange, "Don't clamp boost");
			launchVelocity *= DashStrength;
		}
	}
	else
	{
		launchVelocity *= DashStrength;
	}

	LaunchCharacter(launchVelocity, false, false);
	GetWorld()->GetTimerManager().SetTimer(DashCooldownHandle, this,
	                                       &APlayerCharacter::SetDashCooldownOver, DashCooldown, false);
}

void APlayerCharacter::SetDashCooldownOver()
{
	bIsOnDashCooldown = false;
}

void APlayerCharacter::MantlingTick()
{
	FHitResult Hit;

	float forgiveness = 60.0f;
	FVector EyeLevel = Camera->GetComponentLocation() + FVector(0.0f, 0.0f, forgiveness);
	float forwardDistanceMultiplier = 100.0f;
	FVector TraceStart = EyeLevel + (GetActorForwardVector() * forwardDistanceMultiplier);
	float length = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
	FVector TraceEnd = TraceStart + FVector(0.0f, 0.0f, -1.0f * length);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Visibility;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannelProperty, QueryParams);

	float minMantleHeight = 15.0f;
	float maxMantleHeight = 50.0f + forgiveness;
	FString distance = FString::SanitizeFloat(Hit.Distance);
	bool rightDistance = (Hit.Distance > minMantleHeight) && (Hit.Distance < maxMantleHeight);

	if (Hit.bBlockingHit && rightDistance)
	{
		bCanMantle = true;
	}
	else
	{
		bCanMantle = false;
	}
}

void APlayerCharacter::Mantle()
{
	OnMantle();
	float mantleStrength = 550.0f;
	float forwardStrength = 5.0f;
	FVector launchVelocity = (GetActorForwardVector() * forwardStrength) + FVector(0.0f, 0.0f, mantleStrength);
	LaunchCharacter(launchVelocity, false, false);
}

void APlayerCharacter::CheckFootsteps()
{
	float currentSpeed = GetCharacterMovement()->GetLastUpdateVelocity().Length();
	if (currentSpeed > 10.0f)
	{
		if (IsWallRunning())
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WallrunSound, GetActorLocation());
		}
		else if (!GetCharacterMovement()->IsFalling())
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepSound, GetActorLocation());
		}
	}
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

	bool kioskHit = false;

	for (FHitResult hit : hits)
	{
		APartSelectorKiosk* kiosk = Cast<APartSelectorKiosk>(hit.GetActor());

		if (kiosk)
		{
			kioskHit = true;
			kiosk->RotateDisplayItem(value.Get<float>());
		}
	}

	if (!kioskHit)
	{
		Welder->ScrollPartType(value.Get<float>());
	}
}


void APlayerCharacter::fireInput(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		//trace to check if trying to press finish order or spawn chassis buttons
		FVector eyeLoc;
		FRotator eyeDir;

		GetActorEyesViewPoint(eyeLoc, eyeDir);
		TArray<FHitResult> hits;
		GetWorld()->LineTraceMultiByChannel(hits, GetActorLocation(), GetActorLocation() + (eyeDir.Vector() * 10000.0f),
		                                    ECC_Visibility);
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (eyeDir.Vector() * 10000.0f),
		                          10.0f,
		                          FColor::Green, false, 1.0f);

		for (FHitResult hit : hits)
		{

			/*
			ASpawnChasisActor* spawnChassisButton = Cast<ASpawnChasisActor>(hit.GetActor());
			if (spawnChassisButton != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Spawner hit");

				spawnChassisButton->SpawnChassis();
			}
			*/


			ATicketBoardActor* ticketBoard = Cast<ATicketBoardActor>(hit.GetActor());
			if (ticketBoard) {

				if (!currentlyHeldTicket && ticketBoard->unpluggable) {

					currentlyHeldTicket = ticketBoard->UnplugTicket();
					currentlyHeldTicket->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "TicketSocket");

				}
				else if(currentlyHeldTicket && !ticketBoard->GetPluggedTicket()){

					currentlyHeldTicket->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

					ticketBoard->PlugTicketIn(currentlyHeldTicket);
					currentlyHeldTicket = nullptr;

				}

			}


			AFinishOrderActor* finishOrderButton = Cast<AFinishOrderActor>(hit.GetActor());
			if (finishOrderButton != nullptr)
			{
				finishOrderButton->FinishOrder();
			}
		}

		
	}


	if (value.Get<bool>())
	{
		Welder->WeldInput();
	}
	else
	{
		Welder->WeldReleased();
	}


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

void APlayerCharacter::aimInput(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Welder->BlueprintInput();
	}
	else
	{
		Welder->BlueprintReleased();
	}
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

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LandingSound, GetActorLocation());

	// Restore jumps and end wallrun
	NumOfJumps = 2;
	this->JumpCurrentCount = 0;
	EndWallRun(0.0f);
	bIsWallrunSuppressed = false;

	if (sliding || crouching)
	{
		FVector lateralVelocity = GetCharacterMovement()->Velocity;
		lateralVelocity.Z = 0.0f;

		//if not moving but pressing crouch after jump on a hill/bump, then give minimum slide speed
		if (lateralVelocity.Length() < 450.0f)
		{
			FVector forwardDir = GetActorForwardVector();
			forwardDir.Z = 0.0f;
			forwardDir = forwardDir.GetSafeNormal();

			GetCharacterMovement()->Velocity = forwardDir * 500.0f;
		}


		//change velocity based on the hill steepness and direction
		GetCharacterMovement()->Velocity *= (CalcHillSlideBoost());
	}
}


void APlayerCharacter::Dash()
{
	if (!bIsOnDashCooldown)
	{
		if (IsWallRunning())
		{
			WallRunJump();
		}

		bIsOnDashCooldown = true;
		// We boost the player up to allow the dash to happen
		FVector upBoost = FVector(0.0f, 0.0f, 200.0f);
		LaunchCharacter(upBoost, false, false);

		// Launch the player forward after 0.1 seconds.
		float delay = 0.1f;
		FTimerHandle DashHandle;
		GetWorld()->GetTimerManager().SetTimer(DashHandle, this,
		                                       &APlayerCharacter::PerformDash, delay, false);
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
