// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "VehicleInstancerManager.h"
//#include "TrafficPath.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "TrafficPathControllerTemp.generated.h"

class UTrafficPath;
class UVehicleInstancerManager;
struct FSplineBuilder;

USTRUCT()
struct CIVILFXCORE_API FVehicleControllerTemp
{
	GENERATED_BODY()
		float CurvePos;
	int32 CurrentLane;
	FVehicleInstanceID VehicleID;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CIVILFXCORE_API UTrafficPathControllerTemp : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTrafficPathControllerTemp();

	UPROPERTY(EditAnywhere)
		EPhaseType PhaseType;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowTruck = true;

private:
	UTrafficPath* TrafficPath;
	UVehicleInstancerManager* VehiclesManager;

	TArray<FVehicleControllerTemp> Vehicles;

	void ReSpawnVehicle(float DeltaTime);
	void UpdateVehicles(float DeltaTime);
	void DespawnVehicle();

	void GetWorldTransformFromLaneIndex(FTransform& OutTransform, float CurrentProgress, int32 LaneIndex);
	void AdjustLocationToTerrainMesh(FVector& InOutLocation);

	int32 CurrentSpawnLane;
	int32 LanesCount;
	float PathWidth;
	float SpeedMPH;
	float VehicleDistance;
	float PathLength;
	float GapToRespawn;

	TSharedPtr<FSplineBuilder> Spline;

	void ReCreateAllVehicles();
	void ReleaseAllVehicles();

	bool bVehiclesInitialized;
	void InitializeVehicles();
	UFUNCTION()
		void OnNewPhaseSwitch(EPhaseType NewPhaseType, EPhaseMode NewPhaseMode);
	class APawn* PlayerPawn;
	FVector PawnLocation;

	static CONSTEXPR float MPH2METERSPH = 0.44704f;
};
