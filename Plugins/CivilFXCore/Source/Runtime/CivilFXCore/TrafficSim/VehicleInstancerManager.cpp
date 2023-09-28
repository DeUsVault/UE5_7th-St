// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleInstancerManager.h"
#include "VehicleInstancer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

void UVehicleInstancerManager::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UVehicleInstancerManager::Deinitialize()
{
}

UVehicleInstancerManager::UVehicleInstancerManager() :
	UGameInstanceSubsystem(),
	UniqueBucketID(0)
{

}

void UVehicleInstancerManager::RegisterInstancedActor(AVehicleInstancer* Instancer)
{
	//Add actor to bucket
	Bucket.Add(UniqueBucketID, Instancer);

	//increase uniqueID
	++UniqueBucketID;
}

FVehicleInstanceID UVehicleInstancerManager::CreateVehicle(bool bAllowTruck/*= true*/)
{
	int32 Count = Bucket.Num();

	if (Count == 0)
	{
		return { -1, -1 };
	}

	int32 BucketID = UKismetMathLibrary::RandomIntegerInRange(0, Count - 1);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("BucketID: %d"), BucketID));
	AVehicleInstancer* Instancer = Bucket[BucketID];

	if (!bAllowTruck && Instancer->bIsTruck)
	{
		//Find the first available instancer
		for (const TPair<int32, AVehicleInstancer*> Pair : Bucket)
		{
			if (!Pair.Value->bIsTruck)
			{
				Instancer = Pair.Value;
				BucketID = Pair.Key;
			}
		}
	}

	//@TODO: check(Instancer);
	int32 InstancedIndex = Instancer->GetOrCreateInstance();
	return { BucketID, InstancedIndex };
}

void UVehicleInstancerManager::ReleaseVehicle(FVehicleInstanceID& InOutVehicleID)
{

	if (InOutVehicleID.IsValid())
	{
		Bucket[InOutVehicleID.BucketID]->ReleaseInstance(InOutVehicleID.InstancedIndex);
		InOutVehicleID.BucketID = -1;
		InOutVehicleID.InstancedIndex = -1;
	}
}

void UVehicleInstancerManager::UpdateTransform(const FVehicleInstanceID& InVehicleID, const FTransform& InBodyTransform, float InSpeed)
{
	if (InVehicleID.IsValid())
	{
		Bucket[InVehicleID.BucketID]->UpdateInstanceWorldLocation(InVehicleID.InstancedIndex, InBodyTransform, InSpeed);
	}
}
