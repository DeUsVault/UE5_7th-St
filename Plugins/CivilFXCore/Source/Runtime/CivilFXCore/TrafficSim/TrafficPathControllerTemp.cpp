// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficPathControllerTemp.h"
#include "TrafficPath.h"
#include "GameFramework/Actor.h"
#include "VehicleInstancerManager.h"
#include "Kismet/KismetMathLibrary.h" //LookAt
#include "VehicleInstancer.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UTrafficPathControllerTemp::UTrafficPathControllerTemp() :
	Super()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.01f;
	// ...
}


// Called when the game starts
void UTrafficPathControllerTemp::BeginPlay()
{
	Super::BeginPlay();

	AActor* Actor = GetOwner();
	TrafficPath = (UTrafficPath*)Actor->GetComponentByClass(UTrafficPath::StaticClass());
	VehiclesManager = GetOwner()->GetGameInstance()->GetSubsystem<UVehicleInstancerManager>();
	Spline = TrafficPath->GetSplineBuilder();
	LanesCount = TrafficPath->GetLanesCount();
	PathWidth = TrafficPath->GetPathWidth();
	PathLength = Spline->GetCurveLength();
	VehicleDistance = 50.0f;
	GapToRespawn = VehicleDistance * 2 / LanesCount;
	//meters
	SpeedMPH = UKismetMathLibrary::RandomFloatInRange(30.0f, 50.0f);

	SetComponentTickEnabled(true);
	InitializeVehicles();


	UPhaseManager* PhaseManager = GetOwner()->GetGameInstance()->GetSubsystem<UPhaseManager>();
	PhaseManager->OnSwitchPhase().AddUObject(this, &UTrafficPathControllerTemp::OnNewPhaseSwitch);

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UTrafficPathControllerTemp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	PawnLocation = PlayerPawn->GetActorLocation();
	ReSpawnVehicle(DeltaTime);
	UpdateVehicles(DeltaTime);
	DespawnVehicle();
}

void UTrafficPathControllerTemp::ReSpawnVehicle(float DeltaTime)
{
	if (GapToRespawn <= 0)
	{
		FVehicleInstanceID VehicleID = VehiclesManager->CreateVehicle(bAllowTruck);
		if (VehicleID.IsValid())
		{
			FVehicleControllerTemp VehicleController;
			VehicleController.CurvePos = 0;
			VehicleController.CurrentLane = UKismetMathLibrary::RandomIntegerInRange(0, LanesCount - 1);
			VehicleController.VehicleID = VehicleID;
			Vehicles.Add(VehicleController);
			CurrentSpawnLane = (CurrentSpawnLane + 1) % LanesCount;
		}

		GapToRespawn = TrafficPath->GetPathLength() / 2;
	}
	GapToRespawn -= (DeltaTime * SpeedMPH * MPH2METERSPH);
}

void UTrafficPathControllerTemp::DespawnVehicle()
{
	if (Vehicles.Num() > 0)
	{
		FVehicleControllerTemp VehicleController = Vehicles[0];
		if (VehicleController.CurvePos >= PathLength)
		{
			VehiclesManager->ReleaseVehicle(VehicleController.VehicleID);
			Vehicles.RemoveAt(0);
		}
	}
}

void UTrafficPathControllerTemp::UpdateVehicles(float DeltaTime)
{
	for (FVehicleControllerTemp& VehicleController : Vehicles)
	{
		FTransform VehicleTransform;
		GetWorldTransformFromLaneIndex(VehicleTransform, VehicleController.CurvePos, VehicleController.CurrentLane);
		VehiclesManager->UpdateTransform(VehicleController.VehicleID, VehicleTransform, 30);
		VehicleController.CurvePos += DeltaTime * SpeedMPH * MPH2METERSPH;
	}
}

void UTrafficPathControllerTemp::GetWorldTransformFromLaneIndex(FTransform& OutTransform, float CurvePos, int32 LaneIndex)
{
	FVector CenterStart = Spline->GetPointOnPathSegment(CurvePos);
	FVector Direction = Spline->GetTangentOnPathSegment(CurvePos);
	FVector LeftDir = FVector::CrossProduct(Direction, FVector::UpVector);
	FVector LeftWidth = LeftDir * (PathWidth / 2.0f);
	float Seg = (2.f * LaneIndex + 1.f) / (2.f * LanesCount);

	FVector Location = FMath::Lerp(CenterStart + LeftWidth, CenterStart - LeftWidth, Seg);

	float Dis = FVector::DistSquared(PawnLocation, Location);
	if (Dis <= 5000000000.f) //it's squared distance in cm
	{
		//GLog->Log("Adjusting");
		AdjustLocationToTerrainMesh(Location);
	}

	//AdjustLocationToTerrainMesh(Location);
	//FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Spline->GetPoint(CurrentProgress + 0.001f), Location);
	FRotator Rotator = UKismetMathLibrary::MakeRotationFromAxes(Direction, -LeftDir, FVector::UpVector);

	OutTransform = FTransform(Rotator, Location);
}

void UTrafficPathControllerTemp::AdjustLocationToTerrainMesh(FVector& InOutLocation)
{
	FHitResult Hit(1.0f);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(VehicleLocation), true);
	if (GetWorld()->LineTraceSingleByChannel(Hit, InOutLocation + FVector::UpVector * 100.f, InOutLocation + FVector::DownVector * 1000.f, ECC_WorldStatic, Params))
	{
		InOutLocation = Hit.Location;
	}
}


void UTrafficPathControllerTemp::InitializeVehicles()
{
	GLog->Log("Init");
	for (float Distance = 0; Distance < PathLength; Distance += VehicleDistance * 2)
	{
		FVehicleControllerTemp VehicleController;
		VehicleController.CurvePos = 0;
		VehicleController.CurrentLane = UKismetMathLibrary::RandomIntegerInRange(0, LanesCount - 1);
		VehicleController.VehicleID = VehiclesManager->CreateVehicle(bAllowTruck);
		Vehicles.Insert(VehicleController, 0);
	}
	bVehiclesInitialized = true;
}

void UTrafficPathControllerTemp::OnNewPhaseSwitch(EPhaseType NewPhaseType, EPhaseMode NewPhaseMode)
{
	//GLog->Log(FString::Printf(TEXT("NewPhase for Actor: %s, ThisPhaseType: %s, NewPhaseType: %s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(PhaseType), *UEnum::GetValueAsString(NewPhaseType)));

	if (PhaseType == EPhaseType::None || NewPhaseType == PhaseType)
	{
		if (!bVehiclesInitialized)
		{
			InitializeVehicles();
		}
		else
		{
			ReCreateAllVehicles();
		}
		SetComponentTickEnabled(true);
	}
	else
	{
		ReleaseAllVehicles();
		SetComponentTickEnabled(false);
	}
}
void UTrafficPathControllerTemp::ReCreateAllVehicles()
{
	for (FVehicleControllerTemp& Vehicle : Vehicles)
	{
		if (!Vehicle.VehicleID.IsValid())
		{
			Vehicle.VehicleID = VehiclesManager->CreateVehicle(bAllowTruck);
		}
	}
}

void UTrafficPathControllerTemp::ReleaseAllVehicles()
{
	for (FVehicleControllerTemp& Vehicle : Vehicles)
	{
		VehiclesManager->ReleaseVehicle(Vehicle.VehicleID);
	}
}

/*
//static
bool UTrafficPathControllerTemp::bRequestBeginPlay = false;
void UTrafficPathControllerTemp::RequestBeginPlayOnVehicleInstancer()
{
	return;

	if (!bRequestBeginPlay)
	{
		for (TObjectIterator<AActor> ActorIt; ActorIt; ++ActorIt)
		{
			AVehicleInstancer* Instancer = Cast<AVehicleInstancer>(*ActorIt);
			if (Instancer)
			{
				Instancer->DispatchBeginPlay(true);
			}
		}
		bRequestBeginPlay = true;
	}
}
*/

