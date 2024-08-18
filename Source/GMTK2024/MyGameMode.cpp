// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

#include "Kismet/GameplayStatics.h"

#include "PartBase.h"
#include "ChasisPartBase.h"
#include "StructuralPartBase.h"
#include "FirepowerPartBase.h"
#include "EnginePartBase.h"
#include "EnergyPartBase.h"
#include "SupportPartBase.h"


AMyGameMode::AMyGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	orderSheet = NewObject<UOrderDataSheet>();

	GetWorld()->GetTimerManager().SetTimer(shiftStatTimerHandle, this, &AMyGameMode::ShiftStartCallback, 3.0f);

	localTime = 0.0f;

	shipUnderConstruction = false;
}


void AMyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	localTime += DeltaTime;
}


void AMyGameMode::ShiftStartCallback()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShiftStartAlarm,
	                                      GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(), 1.0f,
	                                      1.0f, 0.0f);

	if (shiftStatTimerHandle.IsValid())
	{
		shiftStatTimerHandle.Invalidate();
	}

	AddOrder();
}


void AMyGameMode::AddOrder()
{	//randomly generate a new order
	FOrder newOrder;
	newOrder.firepower = 100.0f * difficulty;

	if (orderSheet == nullptr)
	{
		// Let's see if this solves my crashing issue
		return;
	}
	orderSheet->orders.Add(newOrder);
	orderSheet->currentOrder = newOrder;

	orderSheet->currentBuildOrder = GetZeroOrder();



	if (addOrderTimerHandle.IsValid())
	{
		addOrderTimerHandle.Invalidate();
	}

	AddOrderBP();

	//GetWorld()->GetTimerManager().SetTimer(addOrderTimerHandle, this, &AMyGameMode::AddOrder,10.0f * (1.0f / difficulty));
}

void AMyGameMode::SpawnShipChasis()
{
}


void AMyGameMode::DoNewShipChassisProcedure()
{
	currentShipChassis->SetActorLocation(shipConstructionLocation);
	shipUnderConstruction = true;
	//MoveShipToLocationOverTime(newShipStartLocation, shipConstructionLocation, transitionTime, 0);
}

void AMyGameMode::MoveShipToLocationOverTime(FVector startLocation, FVector endLocation, float overTime, int nCalls)
{
}





void AMyGameMode::DoFinishOrderProcedure() {

	if (!currentShipChassis) {

		return;

	}

	

	
	FReportCard newReport = EvaluateBuildWithOrder(orderSheet->currentOrder);

	//once attribute grading is done the ship needs to try and fly

	//NEED TO IMPLEMENT THAT PROCEDURE AND THEN CALLBACK TO ADD REPORT AFTERWARDS AS SHOWN BELOW


	//add new report to order data sheet
	orderSheet->reports.Add(newReport);


	//calculate average grade
	float s = 0.0f;
	for (FReportCard report : orderSheet->reports)
	{
		s += report.overall;
	}
	s /= orderSheet->reports.Num();

	orderSheet->averageGrade = s;


	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "ORDER GRADE: " + FString::SanitizeFloat(s));
}




void AMyGameMode::DoShipFlight() {

	//currentShipChassis->SetActorLocation(shipLaunchLocation);


	//activate all of the parts
	TArray<AActor*> partsA;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APartBase::StaticClass(), partsA);

	for (AActor* partA : partsA)
	{
		APartBase* part = Cast<APartBase>(partA);

		part->ActivatePart();
		part->launched = true;
	}


	currentShipChassis->mesh->SetCollisionProfileName("NoCollision");
	currentShipChassis->physicsBox->SetSimulatePhysics(true);

	GetWorld()->GetTimerManager().SetTimer(finsihGradingHandle, this, &AMyGameMode::CompleteGradingAfterFlight, 4.0f);

}



void AMyGameMode::CompleteGradingAfterFlight() {




}







void AMyGameMode::AddPartToBuildOrder(APartBase* part) {

	


	PartType type = part->type;
		
	switch (type){

		case PartType::PT_STRUCTURAL:
			
			orderSheet->currentBuildOrder.structural += part->baseAttribute;

			break;


		case PartType::PT_FIREPOWER:
			
			orderSheet->currentBuildOrder.firepower += part->baseAttribute;

			break;


		case PartType::PT_THRUST:
			
			orderSheet->currentBuildOrder.thrust += part->baseAttribute;

			break;


		case PartType::PT_ENERGY:
			
			orderSheet->currentBuildOrder.energy += part->baseAttribute;

			break;


		case PartType::PT_SUPPORT:
			
			orderSheet->currentBuildOrder.support += part->baseAttribute;

			break;
	}


	orderSheet->currentBuildOrder.cost += part->cost;



	PartAddedBP();
	

}



FReportCard AMyGameMode::EvaluateBuildWithOrder(FOrder order) {

	


	//GRADING BELOW

	TArray<AActor*> partsA;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APartBase::StaticClass(), partsA);
	FOrder currentOrder = orderSheet->orders[orderSheet->orders.Num() - 1];
	FReportCard newReport;

	//sum each part attribute into the report card
	for (AActor* partA : partsA)
	{
		APartBase* part = Cast<APartBase>(partA);
		PartType type = part->type;
		switch (type)
		{
		case PartType::PT_STRUCTURAL:
			newReport.structural += part->baseAttribute;
			break;

		case PartType::PT_FIREPOWER:
			newReport.firepower += part->baseAttribute;
			break;

		case PartType::PT_THRUST:
			newReport.thrust += part->baseAttribute;
			break;

		case PartType::PT_ENERGY:
			newReport.energy += part->baseAttribute;
			break;

		case PartType::PT_SUPPORT:
			newReport.support += part->baseAttribute;
			break;
		}
		newReport.cost += part->cost;

		
		
	}


	
	newReport.harmony = GetHarmonyGrade();


	//grade is 1 minus the percent difference from requested value
	newReport.structural = 1.0f - FMath::Abs((currentOrder.structural - newReport.structural) / currentOrder.structural);
	newReport.firepower = 1.0f - FMath::Abs((currentOrder.firepower - newReport.firepower) / currentOrder.firepower);
	newReport.thrust = 1.0f - FMath::Abs((currentOrder.thrust - newReport.thrust) / currentOrder.thrust);
	newReport.energy = 1.0f - FMath::Abs((currentOrder.energy - newReport.energy) / currentOrder.energy);
	newReport.support = 1.0f - FMath::Abs((currentOrder.support - newReport.support) / currentOrder.support);


	newReport.cost = 1.0f - FMath::Abs((currentOrder.cost - newReport.cost) / currentOrder.cost);


	newReport.overall = (newReport.structural + newReport.firepower + newReport.thrust + newReport.energy + newReport.
		support) / 5.0f;



	newReport.overall *= newReport.harmony;

	return newReport;

}


float AMyGameMode::GetHarmonyGrade() {

	FVector centerOfMass = FVector::ZeroVector;
	float totalMass = 0.0f;

	FVector thrustVector = FVector::ZeroVector;
	FVector centerOfThrust = FVector::ZeroVector;
	float totalThrustMagnitude = 0.0f;
	


	TArray<AActor*> partsA;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APartBase::StaticClass(), partsA);
	






	//get the total thrust and total mass first
	for (AActor* partA : partsA)
	{
		APartBase* part = Cast<APartBase>(partA);
		PartType type = part->type;

		if (type == PartType::PT_THRUST) {
			
			totalThrustMagnitude += part->baseAttribute;


		}

		totalMass += part->mass;
		


	}




	//get the center of mass, center of thrust and thrust vector
	for (AActor* partA : partsA)
	{
		APartBase* part = Cast<APartBase>(partA);
		PartType type = part->type;
		
		if (type == PartType::PT_THRUST) {

			AEnginePartBase* engine = Cast<AEnginePartBase>(part);

			thrustVector += part->GetActorForwardVector() * part->baseAttribute;
			

			centerOfThrust += part->GetActorLocation() * (part->baseAttribute / totalThrustMagnitude);


		}
		
		
		centerOfMass += part->GetActorLocation() * (part->mass / totalMass);


	}

	thrustVector = thrustVector.GetSafeNormal();


	FVector centerOffset = centerOfMass - centerOfThrust;

	//this is how close the thrust vector is to pointing at the center of mass from the center of thrust
	float amountTowardsCOM = FVector::DotProduct(thrustVector, (centerOffset.GetSafeNormal()));

	DrawDebugDirectionalArrow(GetWorld(), currentShipChassis->GetActorLocation() + FVector(0.0f, 0.0f, 500.0f), centerOffset.GetSafeNormal() * 2000.0f, 20.0f, FColor::Green);
	DrawDebugDirectionalArrow(GetWorld(), currentShipChassis->GetActorLocation() + FVector(0.0f, 0.0f, 500.0f), thrustVector * 2000.0f, 20.0f, FColor::Yellow);


	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Harmony Grade: " + FString::SanitizeFloat(amountTowardsCOM));

	return amountTowardsCOM;


}



FOrder AMyGameMode::GetZeroOrder() {

	FOrder blankOrder;

	blankOrder.cost = 0.0f;
	blankOrder.structural = 0.0f;
	blankOrder.firepower = 0.0f;
	blankOrder.thrust = 0.0f;
	blankOrder.energy = 0.0f;
	blankOrder.support = 0.0f;


	return blankOrder;

}