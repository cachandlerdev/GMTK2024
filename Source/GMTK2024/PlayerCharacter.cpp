// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"



#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputLibrary.h"

#include "Kismet/GameplayStatics.h"

#include "UObject/UObjectGlobals.h"


#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	boxCollider = CreateDefaultSubobject<UBoxComponent>("Collision");

	SetRootComponent(boxCollider);


	tempWheelMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Test Wheel");
	

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	DeferSetupMovementSystem();

	boxCollider->SetSimulatePhysics(true);



}





void APlayerCharacter::DeferSetupMovementSystem() {

	playerController = Cast<APlayerController>(GetLocalViewingPlayerController());

	

	//clear the copy mapping context and copy everything from the base defaults to the copy
	baseControlsCopy->UnmapAll();


	TArray<FEnhancedActionKeyMapping> mappings = baseControls->GetMappings();
	for (FEnhancedActionKeyMapping map : mappings) {

		FEnhancedActionKeyMapping* mapCopy = &baseControlsCopy->MapKey(map.Action, map.Key);

		mapCopy->Modifiers = map.Modifiers;



		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, map.Action.GetFName().ToString() + " | " + map.Key.GetFName().ToString());

	}
	UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(baseControlsCopy);



	if (playerController) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());

		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, playerController->GetName());
		FString name = playerController->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *name);

		if (Subsystem) {
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(baseControlsCopy, baseControlsPriority);
		}



		movementSuccessful = true;

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, GetName() + "Couldn't set up movement system.");
	}





}


// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	playerEnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerEnhancedInput) {

		if (movementAction && lookAction) {
			//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, "valid");



			//BUILD ALL MAPPINGS THAT ARENT BUILT IN EDITOR

			//baseControlsCopy->MapKey(fireAction, EKeys::LeftMouseButton);

			//need to rebuild the control mapping
			//UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(baseControlsCopy);




			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, "Binding");



			playerEnhancedInput->BindAction(movementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::moveInput);
			//playerEnhancedInput->BindAction(airStrafeAction, ETriggerEvent::Started, this, &APlayerCharacter::airStrafeInput);
			//playerEnhancedInput->BindAction(forwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::forwardInput);


			playerEnhancedInput->BindAction(lookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::lookInput);
			//playerEnhancedInput->BindAction(zoomAction, ETriggerEvent::Triggered, this, &APlayerCharacter::zoomInput);

			//playerEnhancedInput->BindAction(sprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::sprintInput);
			//playerEnhancedInput->BindAction(jumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::jumpInput);

			//playerEnhancedInput->BindAction(crouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::crouchInput);

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

}



void APlayerCharacter::moveInput(const FInputActionValue& value) {

	//GetControlRotation().Vector().ForwardVector;

	FVector straight = GetControlRotation().Vector();

	FVector right = GetControlRotation().Vector().Cross(-FVector::UpVector);
	FVector forward = right.Cross(FVector::UpVector);

	forward.Z = 0.0f;
	forward = forward.GetSafeNormal();

	right.Z = 0.0f;
	right = right.GetSafeNormal();

	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, "MoveInput");

	AddMovementInput(forward, value.Get<FVector2D>().X * 10.0f);
	AddMovementInput(right, value.Get<FVector2D>().Y * 10.0f);

	//inputHorizontalDirection = ((forward * value.Get<FVector2D>().X) + (right * value.Get<FVector2D>().Y)).GetSafeNormal();

	//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (GetControlRotation().Vector() * 100.0f), 10.0f, FColor::White);
	//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (forward * 100.0f), 10.0f, FColor::Green);
	//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (right * 100.0f), 10.0f, FColor::Red);


}

void APlayerCharacter::lookInput(const FInputActionValue& value) {

	AddControllerPitchInput(-1.0f * value.Get<FVector2D>().Y * lookSensitivity);
	AddControllerYawInput(value.Get<FVector2D>().X * lookSensitivity);

}



void APlayerCharacter::sprintInput(const FInputActionValue& value) {

	//sprinting = value.Get<bool>();

}

void APlayerCharacter::jumpInput(const FInputActionValue& value) {

	//Jump();



}



void APlayerCharacter::fireInput(const FInputActionValue& value) {

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Base Fire Called");


}

void APlayerCharacter::heavyFireInput(const FInputActionValue& value) {



}





void APlayerCharacter::crouchInput(const FInputActionValue& value) {

}


void APlayerCharacter::dodgeInput(const FInputActionValue& value) {



}

void APlayerCharacter::targetLockInput(const FInputActionValue& value) {



}





void APlayerCharacter::aimInput(const FInputActionValue& value) {



}

void APlayerCharacter::ability1Input(const FInputActionValue& value) {

	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, "ability1 call");

}













void APlayerCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const {

	TagContainer = GameplayTags;

}
