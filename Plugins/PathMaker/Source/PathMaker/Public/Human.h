// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Human.generated.h"

class USkeletalMeshComponent;

UCLASS()
class PATHMAKER_API AHuman : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHuman();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float CurrentSplinetime;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Initialize();
	UFUNCTION()
		float CalculateDistance(float currentTimeSeconds);
	UFUNCTION()
		void  SetCurrentSplineTime(float splineTime) { CurrentSplinetime = splineTime;  };
	UFUNCTION()
		float GetCurrentSplineTime() { return CurrentSplinetime; };
	UFUNCTION()
		void AdjustOrientation();



	UPROPERTY()
		float StartTime;
	UPROPERTY()
		bool bCanMoveObject;
	/*UPROPERTY(EditAnywhere, Category = "Path Maker")
		float startOffset;*/
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float speed;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float offsetFromSpline;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool MovingFromSplineStart;

};
