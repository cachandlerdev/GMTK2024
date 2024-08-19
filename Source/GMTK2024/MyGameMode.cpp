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


#include "TicketActor.h"
#include "TicketBoardActor.h"




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
	
	
	//clear ticket array
	tickets.Empty();


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
	
	if (tickets.Num() < 3) {




		FRandomStream r;

		r.GenerateNewSeed();

		ATicketActor* newTicket = Cast<ATicketActor>(GetWorld()->SpawnActor<ATicketActor>(ticketSpawnClass));

		newTicket->ticketID = tickets.Num();
		tickets.Add(newTicket);

		//randomly generate a new order
		FOrder newOrder;
		newOrder.cost = r.FRandRange(0.0f, 300.0f) * difficulty;
		newOrder.structural = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.firepower = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.thrust = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.energy = r.FRandRange(0.0f, 100.0f) * difficulty;
		newOrder.support = r.FRandRange(0.0f, 100.0f) * difficulty;

		newOrder.customerPatience = r.FRandRange(10.0f, 60.0f / difficulty);

		newOrder.companyName = companyNames[((int)r.RandRange(0.0f, ((float)companyNames.Num()) - 0.01f))];

		int orderShipType = (int)(r.FRandRange(0.0f, 3.99f));
		//newOrder.shipType = static_cast<ShipType>(orderShipType);


		newTicket->order = newOrder;

		newTicket->progressOrder = GetZeroOrder();



		buildTime = 0.01f;




		TArray<AActor*> boardsA;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATicketBoardActor::StaticClass(), boardsA);

		for (AActor* boardA : boardsA) {
			ATicketBoardActor* board = Cast<ATicketBoardActor>(boardA);
			
			if (board->unpluggable && !board->GetPluggedTicket()) {

				board->PlugTicketIn(newTicket);

			}

		}



		

		AddOrderBP();


	}

	
	if (addOrderTimerHandle.IsValid())
	{
		addOrderTimerHandle.Invalidate();
	}
	GetWorld()->GetTimerManager().SetTimer(addOrderTimerHandle, this, &AMyGameMode::AddOrder,10.0f * (1.0f / difficulty));
}





void AMyGameMode::DoNewShipChassisProcedure(TSubclassOf<AChasisPartBase> chassisType, ATicketActor* ticket, AActor* bayTransformActor)
{

	nShipsUnderConstruction++;

	ticket->shipChassis = GetWorld()->SpawnActor<AChasisPartBase>(chassisType, bayTransformActor->GetActorTransform());
	ticket->shipChassis->owningTicket = ticket;
	
}

void AMyGameMode::MoveShipToLocationOverTime(FVector startLocation, FVector endLocation, float overTime, int nCalls)
{
}





void AMyGameMode::DoFinishOrderProcedure(ATicketActor* ticket) {

	if (!ticket->shipChassis || finishingOrder) {

		return;

	}

	finishingOrder = true;

	
	reportInProgress = EvaluateBuildWithOrder(ticket);

	//once attribute grading is done the ship needs to try and fly

	//NEED TO IMPLEMENT THAT PROCEDURE AND THEN CALLBACK TO ADD REPORT AFTERWARDS AS SHOWN BELOW
	DoShipFlight(ticket);


	



	
}




void AMyGameMode::DoShipFlight(ATicketActor* ticket) {

	ticket->shipChassis->SetActorLocation(shipLaunchLocation);


	//get the ship thrust vector to pass to the engines
	FVector shipThrustVector;
	FVector shipCenterOfThrust;

	GetShipThrust(shipCenterOfThrust, shipThrustVector, ticket);




	TArray<APartBase*> parts = ticket->shipChassis->childParts;



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

	FVector currentCOM = ticket->shipChassis->physicsBox->GetCenterOfMass();
	FVector newCOM = GetShipCenterOfMass(ticket);
	FVector offsetCOM = ticket->shipChassis->GetActorRotation().UnrotateVector(newCOM - currentCOM);
	
	

	//setup the ship for flight physics
	ticket->shipChassis->physicsBox->SetCenterOfMass(offsetCOM);
	ticket->shipChassis->physicsBox->SetSimulatePhysics(true);
	ticket->shipChassis->physicsBox->SetEnableGravity(false);

	ticket->shipChassis->physicsBox->SetCollisionProfileName("BlockAll");

	ticket->shipChassis->physicsBox->SetAngularDamping(baseAngularDamping);

	ticket->shipChassis->mesh->SetCollisionProfileName("NoCollision");
	
	



	//this is how to do timers wtih functions that have parameters
	FTimerDelegate tempDelegate;

	tempDelegate.BindUFunction(this, FName("CompleteGradingAfterFlight"), ticket);

	GetWorld()->GetTimerManager().SetTimer(finsihGradingHandle, tempDelegate, maxFlightTime, false);

	

}



void AMyGameMode::CompleteGradingAfterFlight(ATicketActor* ticket) {


	//add new report to order data sheet
	orderSheet->reports.Add(reportInProgress);


	//calculate average grade
	float s = 0.0f;
	for (FReportCard& report : orderSheet->reports)
	{
		s += report.overall;
	}
	s /= orderSheet->reports.Num();

	orderSheet->averageGrade = s;

	CleanupShip(ticket);

	shiftQuota--;
	UpdateQuotaBP();

	if (shiftQuota < 1) {

		EndShiftProcedure();

	}


	ATicketActor* temp = ticket;
	tickets.Remove(ticket);

	temp->Destroy();



	finishingOrder = false;



	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "ORDER GRADE: " + FString::SanitizeFloat(s));

}




void AMyGameMode::CleanupShip(ATicketActor* ticket) {

	TArray<APartBase*> parts = ticket->shipChassis->childParts;

	//get the total thrust and total mass first
	for (APartBase* part : parts)
	{
		part->Destroy();

	}

	//dont shrink because we want to always be able to access each array element
	ticket->shipChassis = nullptr;
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
















void AMyGameMode::AddPartToBuild(APartBase* part, ATicketActor* ticket) {

	


	PartType type = part->type;
		
	switch (type){

		case PartType::PT_STRUCTURAL:
			
			ticket->progressOrder.structural += part->baseAttribute;

			break;


		case PartType::PT_FIREPOWER:
			
			ticket->progressOrder.firepower += part->baseAttribute;

			break;


		case PartType::PT_THRUST:
			
			ticket->progressOrder.thrust += part->baseAttribute;

			break;


		case PartType::PT_ENERGY:
			
			ticket->progressOrder.energy += part->baseAttribute;

			break;


		case PartType::PT_SUPPORT:
			
			ticket->progressOrder.support += part->baseAttribute;

			break;
	}


	ticket->progressOrder.cost += part->cost;



	PartAddedBP();
	

}



FReportCard AMyGameMode::EvaluateBuildWithOrder(ATicketActor* ticket) {

	


	//GRADING BELOW

	TArray<APartBase*> parts = ticket->shipChassis->childParts;


	//FOrder currentOrder = orderSheet->orders[orderSheet->orders.Num() - 1];
	FReportCard newReport;


	newReport.companyName = ticket->order.companyName;

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


	
	newReport.harmony = GetHarmonyGrade(ticket);


	//grade is 1 minus the percent difference from requested value
	newReport.structural = FMath::Clamp(1.0f - FMath::Abs((ticket->order.structural - newReport.structural) / ticket->order.structural), 0.0f, 1.0f);
	newReport.firepower = FMath::Clamp(1.0f - FMath::Abs((ticket->order.firepower - newReport.firepower) / ticket->order.firepower), 0.0f, 1.0f);
	newReport.thrust = FMath::Clamp(1.0f - FMath::Abs((ticket->order.thrust - newReport.thrust) / ticket->order.thrust), 0.0f, 1.0f);
	newReport.energy = FMath::Clamp(1.0f - FMath::Abs((ticket->order.energy - newReport.energy) / ticket->order.energy), 0.0f, 1.0f);
	newReport.support = FMath::Clamp(1.0f - FMath::Abs((ticket->order.support - newReport.support) / ticket->order.support), 0.0f, 1.0f);


	newReport.cost = 1.0f - FMath::Abs((ticket->order.cost - newReport.cost) / ticket->order.cost);


	newReport.customerPatience = FMath::Clamp(ticket->order.customerPatience / ticket->ticketTime, 0.0f, 1.0f);

	


	newReport.overall = (newReport.structural + newReport.firepower + newReport.thrust + newReport.energy + newReport.
		support + newReport.customerPatience) / 6.0f;



	newReport.overall *= newReport.harmony;
	
	
	if (ticket->order.shipType != ticket->shipChassis->shipType) {
		newReport.overall *= 0.6f;
	}


	return newReport;

}





float AMyGameMode::GetHarmonyGrade(ATicketActor* ticket) {


	if (!ticket->shipChassis) {

		return 0.0f;

	}

	FVector centerOfMass = FVector::ZeroVector;
	float totalMass = 0.0f;

	FVector thrustVector = FVector::ZeroVector;
	FVector centerOfThrust = FVector::ZeroVector;
	float totalThrustMagnitude = 0.0f;
	





	TArray<APartBase*> parts = ticket->shipChassis->childParts;
	

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



FVector AMyGameMode::GetShipCenterOfMass(ATicketActor* ticket) {

	if (!ticket->shipChassis) {

		return FVector::ZeroVector;

	}

	FVector centerOfMass = FVector::ZeroVector;
	float totalMass = 0.0f;


	TArray<APartBase*> parts = ticket->shipChassis->childParts;


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




void AMyGameMode::GetShipThrust(FVector& centerOfThrust, FVector& thrustVector, ATicketActor* ticket) {


	if (!ticket->shipChassis) {

		return;

	}

	
	float totalThrustMagnitude = 0.0f;



	TArray<APartBase*> parts = ticket->shipChassis->childParts;


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