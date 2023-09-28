// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "TrainControllerComponent.generated.h"

class USplineComponent;
class AMovingObjectActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PATHMAKER_API UTrainControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrainControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		void MovingObjectRespawn();	
	UFUNCTION()
		void UpdatePosition();

	// These are the objects that are being spawned
	TArray<AMovingObjectActor*> MovingObject;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Path Maker")
		EPhaseType PhaseType;
	// These are the variety of train to spawn
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		TArray<TSubclassOf<AActor>> MovingObjectClass;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float SecondsToCompletePath = DEFAULT_TIME_TO_COMPLETE;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float PauseTimeInSeconds = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool bPauseInBetweenLoops = true;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float DelayTime = 1.0f;


	UFUNCTION()
		void OnNewPhaseSwitch(EPhaseType NewPhaseType, EPhaseMode NewPhaseMode);
	UFUNCTION()
		void HideAllTrainCars();

private:
	const float DEFAULT_TIME_TO_COMPLETE = 90.0f;

	UPhaseManager* PhaseManager;

	USplineComponent* SplineFromParent;
	bool bParentSplineIsLooping;
	float CurrentSplineTime = 0.0f;

	float StartTime;
	
	UFUNCTION()
	void DelayTrain();

	UPROPERTY(Transient)
	TArray<AActor*> CachedTrains;

	FTimerHandle MovingObjectRespawnHandler;
	FTimerHandle UpdatePositionHandler;
	FTimerHandle DelayHandler;
};
