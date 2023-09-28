// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingObjectActor.generated.h"

class UStaticMeshComponent;
UCLASS()
class PATHMAKER_API AMovingObjectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingObjectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		float StartTime;
	UPROPERTY()
		bool bCanMoveObject;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float offset;
};
