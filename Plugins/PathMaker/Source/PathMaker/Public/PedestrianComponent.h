// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "PedestrianComponent.generated.h"

class USplineComponent;
class AHuman;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PATHMAKER_API UPedestrianComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPedestrianComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ChangeSpawnTime();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	DECLARE_DELEGATE_OneParam(OnSpawnTimerChangeDelegate, float);
	OnSpawnTimerChangeDelegate spawnTimerChangeDelegate;

	UPROPERTY(EditAnywhere, Category = "Path Maker")
	 	EPhaseType PhaseType;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool bLoopPedestrians;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool bTwoWayPedestrian;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool bTrackFloor = true;
	// These are the variety of humans to spawn
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		TArray<TSubclassOf<AActor>> MovingObjectClass;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		int MaxNumOfPeople = TOTAL_NUM_OF_PED;
	UPROPERTY(EditAnywhere, Category = "Path Maker", meta = (UIMin = "0.5", UIMax = "6.0"))
		float SpawnTimerMin = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Path Maker", meta = (UIMin = "7.0", UIMax = "20.0"))
		float SpawnTimerMax = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Path Maker", meta = (UIMin = "0.0", UIMax = "50.0"))
		float offsetFromSpline = 25.0f;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		//float PauseTimeInSeconds = 5.0f;

	// These are the humans that are being spawned
	TArray<AHuman*> MovingObject;


	/* Update the position of all stored pedestrian*/
	UFUNCTION()
		void UpdatePosition();
	UFUNCTION()
		void SpawnAMover(TSubclassOf<AActor> moc);
	UFUNCTION()
		void ShouldSpawnAMover();
	UFUNCTION()
		void MovingObjectRespawn();
	UFUNCTION()
		void TestFunction();
	UFUNCTION()
		void OnSpawnTimerChange();
	UFUNCTION()
		void OnNewPhaseSwitch(EPhaseType NewPhaseType, EPhaseMode NewPhaseMode);
	UFUNCTION(BlueprintCallable)
		void ResetLocation();
	UFUNCTION(BlueprintCallable)
		void HideAllPedestrians();

private:
	

	static const int TOTAL_NUM_OF_PED = 10;
	//FTimerHandle ShouldSpawnMoverHandler;
	int CurrentMoverIndex; 

	USplineComponent* SplineFromParent;
	//bool bParentSplineIsLooping;
	float CurrentSpawnTime;
	FTimerHandle ShouldSpawnMoverHandler;
	FTimerHandle UpdatePositionHandler;
	FTimerHandle MovingObjectRespawnHandler;

	UFUNCTION()
	FVector AdjustLocationToFloor(FVector location);


};
