// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingObjectActor.h"

// Sets default values
AMovingObjectActor::AMovingObjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMovingObjectActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartTime = 0.0f;
	bCanMoveObject = false;
}

// Called every frame
void AMovingObjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

