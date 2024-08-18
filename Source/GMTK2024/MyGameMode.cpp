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

	if (addOrderTimerHandle.IsValid())
	{
		addOrderTimerHandle.Invalidate();
	}

	GetWorld()->GetTimerManager().SetTimer(addOrderTimerHandle, this, &AMyGameMode::AddOrder,
	                                       10.0f * (1.0f / difficulty));
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

	currentShipChassis->SetActorLocation(shipLaunchLocation);

	
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


FReportCard AMyGameMode::EvaluateBuildWithOrder(FOrder order) {


	//GRADING BELOW

	TArray<AActor*> partsA;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APartBase::StaticClass(), partsA);
	FOrder currentOrder = orderSheet->orders[orderSheet->orders.Num() - 1];
	FReportCard newReport;

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

		//part->ActivatePart();
		//part->launched = true;
	}


	currentShipChassis->mesh->SetCollisionProfileName("NoCollision");
	currentShipChassis->physicsBox->SetSimulatePhysics(true);


	//grade is 1 minus the percent difference from requested value
	newReport.structural = 1.0f - FMath::Abs((currentOrder.structural - newReport.structural) / currentOrder.structural);
	newReport.firepower = 1.0f - FMath::Abs((currentOrder.firepower - newReport.firepower) / currentOrder.firepower);
	newReport.thrust = 1.0f - FMath::Abs((currentOrder.thrust - newReport.thrust) / currentOrder.thrust);
	newReport.energy = 1.0f - FMath::Abs((currentOrder.energy - newReport.energy) / currentOrder.energy);
	newReport.support = 1.0f - FMath::Abs((currentOrder.support - newReport.support) / currentOrder.support);


	newReport.cost = 1.0f - FMath::Abs((currentOrder.cost - newReport.cost) / currentOrder.cost);


	newReport.overall = (newReport.structural + newReport.firepower + newReport.thrust + newReport.energy + newReport.
		support) / 5.0f;


	return newReport;

}