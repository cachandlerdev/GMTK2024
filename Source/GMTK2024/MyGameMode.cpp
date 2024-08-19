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
	PrimaryActorTick.bCanEverTick = true;

}


void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	orderSheet = NewObject<UOrderDataSheet>();

	GetWorld()->GetTimerManager().SetTimer(shiftStatTimerHandle, this, &AMyGameMode::ShiftStartCallback, 3.0f);

	localTime = 0.0f;

	
	
	
	
	
	
	nShipsUnderConstruction = 0;
	
	
	//setup the current ship build array
	currentShipChassis.Empty();

	currentShipChassis.SetNum(4);

	for (AChasisPartBase* chassis : currentShipChassis) {

		if (chassis) {

			chassis->Destroy();

		}

		chassis = nullptr;

	}
}


void AMyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	localTime += DeltaTime;

	buildTime += DeltaTime;

	
}


void AMyGameMode::ShiftStartCallback()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShiftStartAlarm,
	                                      GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(), 1.0f,
	                                      1.0f, 0.0f);



	FRandomStream r;
	r.GenerateNewSeed();

	shiftQuota = (int)r.FRandRange(1.0f, 2.0f);
	UpdateQuotaBP();


	if (shiftStatTimerHandle.IsValid())
	{
		shiftStatTimerHandle.Invalidate();
	}

	AddOrder();
}


void AMyGameMode::AddOrder(){	
	
	if (orderSheet->currentOrders.Num() < 3) {
		FRandomStream r;

		r.GenerateNewSeed();



		//randomly generate a new order
		FOrder newOrder;
		newOrder.cost = r.FRandRange(0.0f, 300.0f) * difficulty;
		newOrder.structural = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.firepower = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.thrust = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.energy = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.support = r.FRandRange(0.0f, 100.0f) * difficulty;

		newOrder.customerPatience = r.FRandRange(10.0f, 60.0f / difficulty);



		buildTime = 0.01f;


		if (orderSheet == nullptr)
		{
			// Let's see if this solves my crashing issue
			return;
		}
		orderSheet->orders.Add(newOrder);
		orderSheet->currentOrders.Add(newOrder);

		orderSheet->currentBuildOrder = GetZeroOrder();



		if (addOrderTimerHandle.IsValid())
		{
			addOrderTimerHandle.Invalidate();
		}

		AddOrderBP();


	}

	

	GetWorld()->GetTimerManager().SetTimer(addOrderTimerHandle, this, &AMyGameMode::AddOrder,10.0f * (1.0f / difficulty));
}





void AMyGameMode::DoNewShipChassisProcedure(int shipID)
{

	nShipsUnderConstruction++;

	//currentShipChassis[shipID]->SetActorLocation(shipConstructionLocation);
	//currentShipChassis[shipID]->SetActorRotation(FVector(0.0f, -1.0f, 0.0f).Rotation());
	
	currentShipChassis[shipID]->shipID =  shipID;

	//MoveShipToLocationOverTime(newShipStartLocation, shipConstructionLocation, transitionTime, 0);
}

void AMyGameMode::MoveShipToLocationOverTime(FVector startLocation, FVector endLocation, float overTime, int nCalls)
{
}





void AMyGameMode::DoFinishOrderProcedure(int shipID, int ticketID) {

	if (!currentShipChassis[shipID] || finishingOrder) {

		return;

	}

	finishingOrder = true;

	
	reportInProgress = EvaluateBuildWithOrder(orderSheet->currentOrders[ticketID], shipID);

	//once attribute grading is done the ship needs to try and fly

	//NEED TO IMPLEMENT THAT PROCEDURE AND THEN CALLBACK TO ADD REPORT AFTERWARDS AS SHOWN BELOW
	DoShipFlight(shipID);


	
}




void AMyGameMode::DoShipFlight(int shipID) {

	currentShipChassis[shipID]->SetActorLocation(shipLaunchLocation);


	//get the ship thrust vector to pass to the engines
	FVector shipThrustVector;
	FVector shipCenterOfThrust;

	GetShipThrust(shipCenterOfThrust, shipThrustVector, shipID);




	TArray<APartBase*> parts = currentShipChassis[shipID]->childParts;



	for (APartBase* part : parts)
	{
		
		PartType type = part->type;

		if (type == PartType::PT_THRUST) {

			Cast<AEnginePartBase>(part)->thrustVector = shipThrustVector;
			Cast<AEnginePartBase>(part)->centerOfThrust = shipCenterOfThrust;

		}

		part->ActivatePart();
		part->launched = true;


	}

	FVector currentCOM = currentShipChassis[shipID]->physicsBox->GetCenterOfMass();
	FVector newCOM = GetShipCenterOfMass(shipID);
	FVector offsetCOM = currentShipChassis[shipID]->GetActorRotation().UnrotateVector(newCOM - currentCOM);
	
	

	//setup the ship for flight physics
	currentShipChassis[shipID]->physicsBox->SetCenterOfMass(offsetCOM);
	currentShipChassis[shipID]->physicsBox->SetSimulatePhysics(true);
	currentShipChassis[shipID]->physicsBox->SetEnableGravity(false);

	currentShipChassis[shipID]->physicsBox->SetCollisionProfileName("BlockAll");

	currentShipChassis[shipID]->physicsBox->SetAngularDamping(baseAngularDamping);

	currentShipChassis[shipID]->mesh->SetCollisionProfileName("NoCollision");
	
	



	//this is how to do timers wtih functions that have parameters
	FTimerDelegate tempDelegate;

	tempDelegate.BindUFunction(this, FName("CompleteGradingAfterFlight"), shipID);

	GetWorld()->GetTimerManager().SetTimer(finsihGradingHandle, tempDelegate, maxFlightTime, false);

	

}



void AMyGameMode::CompleteGradingAfterFlight(int shipID) {


	//add new report to order data sheet
	orderSheet->reports.Add(reportInProgress);


	//calculate average grade
	float s = 0.0f;
	for (FReportCard report : orderSheet->reports)
	{
		s += report.overall;
	}
	s /= orderSheet->reports.Num();

	orderSheet->averageGrade = s;

	CleanupShip(shipID);

	shiftQuota--;
	UpdateQuotaBP();

	if (shiftQuota < 1) {

		EndShiftProcedure();

	}

	finishingOrder = false;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "ORDER GRADE: " + FString::SanitizeFloat(s));

}




void AMyGameMode::CleanupShip(int shipID) {

	TArray<APartBase*> parts = currentShipChassis[shipID]->childParts;

	//get the total thrust and total mass first
	for (APartBase* part : parts)
	{
		part->Destroy();

	}

	//dont shrink because we want to always be able to access each array element
	currentShipChassis[shipID] = nullptr;
	nShipsUnderConstruction--;

}




void AMyGameMode::EndShiftProcedure() {
	OnShiftEnd();
	//orderSheet->reports.Empty();

	if (shiftStatTimerHandle.IsValid()) {

		shiftStatTimerHandle.Invalidate();

	}

	GetWorld()->GetTimerManager().SetTimer(shiftStatTimerHandle, this, &AMyGameMode::ShiftStartCallback, 3.0f);

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



FReportCard AMyGameMode::EvaluateBuildWithOrder(FOrder order, int shipID) {

	


	//GRADING BELOW

	TArray<APartBase*> parts = currentShipChassis[shipID]->childParts;


	FOrder currentOrder = orderSheet->orders[orderSheet->orders.Num() - 1];
	FReportCard newReport;

	//sum each part attribute into the report card
	for (APartBase* part : parts)
	{
		
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


	
	newReport.harmony = GetHarmonyGrade(shipID);


	//grade is 1 minus the percent difference from requested value
	newReport.structural = 1.0f - FMath::Abs((currentOrder.structural - newReport.structural) / currentOrder.structural);
	newReport.firepower = 1.0f - FMath::Abs((currentOrder.firepower - newReport.firepower) / currentOrder.firepower);
	newReport.thrust = 1.0f - FMath::Abs((currentOrder.thrust - newReport.thrust) / currentOrder.thrust);
	newReport.energy = 1.0f - FMath::Abs((currentOrder.energy - newReport.energy) / currentOrder.energy);
	newReport.support = 1.0f - FMath::Abs((currentOrder.support - newReport.support) / currentOrder.support);


	newReport.cost = 1.0f - FMath::Abs((currentOrder.cost - newReport.cost) / currentOrder.cost);


	newReport.customerPatience = FMath::Clamp(currentOrder.customerPatience / buildTime, 0.0f, 1.0f);


	newReport.overall = (newReport.structural + newReport.firepower + newReport.thrust + newReport.energy + newReport.
		support + newReport.customerPatience) / 6.0f;



	newReport.overall *= newReport.harmony;

	return newReport;

}





float AMyGameMode::GetHarmonyGrade(int shipID) {


	if (!currentShipChassis[shipID]) {

		return 0.0f;

	}

	FVector centerOfMass = FVector::ZeroVector;
	float totalMass = 0.0f;

	FVector thrustVector = FVector::ZeroVector;
	FVector centerOfThrust = FVector::ZeroVector;
	float totalThrustMagnitude = 0.0f;
	





	TArray<APartBase*> parts = currentShipChassis[shipID]->childParts;
	

	//get the total thrust and total mass first
	for (APartBase* part : parts)
	{
		
		PartType type = part->type;

		if (type == PartType::PT_THRUST) {
			
			totalThrustMagnitude += part->baseAttribute;


		}

		totalMass += part->mass;
		


	}




	//get the center of mass, center of thrust and thrust vector
	for (APartBase* part : parts)
	{
		
		PartType type = part->type;
		
		if (type == PartType::PT_THRUST) {

			AEnginePartBase* engine = Cast<AEnginePartBase>(part);

			thrustVector += part->GetActorForwardVector() * part->baseAttribute;
			
			//DrawDebugDirectionalArrow(GetWorld(), part->GetActorLocation(), GetActorLocation(), 20.0f, FColor::Green);


			centerOfThrust += part->GetActorLocation() * (part->baseAttribute / totalThrustMagnitude);


		}
		
		
		centerOfMass += part->GetActorLocation() * (part->mass / totalMass);


	}

	thrustVector = thrustVector.GetSafeNormal();


	FVector centerOffset = centerOfMass - centerOfThrust;


	//this is how close the thrust vector is to pointing at the center of mass from the center of thrust
	float amountTowardsCOM = FVector::DotProduct(-thrustVector, (centerOffset.GetSafeNormal()));

	//DrawDebugSphere(GetWorld(), centerOfMass, 50.0f, 32, FColor::Green);

	//DrawDebugDirectionalArrow(GetWorld(), centerOfThrust, centerOfMass + centerOffset, 20.0f, FColor::Green);
	//DrawDebugDirectionalArrow(GetWorld(), centerOfThrust, centerOfThrust + (-thrustVector * 500.0f), 20.0f, FColor::Yellow);


	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "Harmony Grade: " + FString::SanitizeFloat(amountTowardsCOM));

	return amountTowardsCOM;


}



FVector AMyGameMode::GetShipCenterOfMass(int shipID) {

	if (!currentShipChassis[shipID]) {

		return FVector::ZeroVector;

	}

	FVector centerOfMass = FVector::ZeroVector;
	float totalMass = 0.0f;


	TArray<APartBase*> parts = currentShipChassis[shipID]->childParts;


	//get the total thrust and total mass first
	for (APartBase* part : parts)
	{
		
		totalMass += part->mass;

	}




	//get the center of mass, center of thrust and thrust vector
	for (APartBase* part : parts)
	{
		
		centerOfMass += part->GetActorLocation() * (part->mass / totalMass);

	}

	


	return centerOfMass;

}




void AMyGameMode::GetShipThrust(FVector& centerOfThrust, FVector& thrustVector, int shipID) {


	if (!currentShipChassis[shipID]) {

		return;

	}

	
	float totalThrustMagnitude = 0.0f;



	TArray<APartBase*> parts = currentShipChassis[shipID]->childParts;


	//get the total thrust and total mass first
	for (APartBase* part : parts)
	{
		
		PartType type = part->type;

		if (type == PartType::PT_THRUST) {

			totalThrustMagnitude += part->baseAttribute;


		}

		



	}




	//get the center of mass, center of thrust and thrust vector
	for (APartBase* part : parts)
	{
		
		PartType type = part->type;

		if (type == PartType::PT_THRUST) {

			AEnginePartBase* engine = Cast<AEnginePartBase>(part);

			thrustVector += part->GetActorForwardVector() * part->baseAttribute;

			


			centerOfThrust += part->GetActorLocation() * (part->baseAttribute / totalThrustMagnitude);


		}





	}

	thrustVector = thrustVector.GetSafeNormal();

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