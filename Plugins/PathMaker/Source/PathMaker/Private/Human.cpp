// Fill out your copyright notice in the Description page of Project Settings.


#include "Human.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AHuman::AHuman()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


}

// Called when the game starts or when spawned
void AHuman::BeginPlay()
{
	Super::BeginPlay();
	MovingFromSplineStart = true;
	offsetFromSpline = 25;
	
}

// Called every frame
void AHuman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AHuman::Initialize()
{

	//this->GetWorld()->GetTimerManager().SetTimer(UpdatePositionHandler, this, &APathSpline::UpdatePosition, 0.02f, true, -1.0f);
}

float AHuman::CalculateDistance(float currentTimeSeconds)
{
	return (speed * (currentTimeSeconds - StartTime)); // -startOffset;
}

void AHuman::AdjustOrientation()
{
	FTransform tempTransform = this->GetActorTransform();
	this->SetActorRotation(FRotator(0.0f, 180, 0.0f));
	

}

