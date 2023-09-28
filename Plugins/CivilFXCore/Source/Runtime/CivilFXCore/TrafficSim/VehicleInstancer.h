// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FoliageInstancedStaticMeshComponent.h"

#include "Engine/Engine.h"
#include "VehicleInstancer.generated.h"



//class UInstancedStaticMeshComponent;
//class UInstancedStaticMeshComponent;

class UVehicleInstancerManager;
class AVehicleInstancer;

class UStaticMeshComponent;
class UVehicleInstancerMeasureTool;

USTRUCT(BlueprintType, Blueprintable)
struct FVehicleWheelMesh
{
	GENERATED_USTRUCT_BODY()
public:
	FVehicleWheelMesh() :
		OffsetToBody(FVector::ZeroVector)
	{

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* WheelMesh = nullptr;

	UPROPERTY(EditAnywhere)
	FVector OffsetToBody = FVector (10.f);
	UPROPERTY(EditAnywhere)
	FRotator OffsetAngle =  FRotator (10.f);

	FORCEINLINE void BeginPlay()
	{
		//BaseTransform = FTransform(OffsetAngle, OffsetToBody, FVector::OneVector);
		BaseRotator = FQuat(OffsetAngle);
		
		WheelMesh->Modify(false);
		WheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WheelMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		WheelMesh->SetGenerateOverlapEvents(false);
		WheelMesh->SetCanEverAffectNavigation(false);
		WheelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

		WheelMesh->SetShouldUpdatePhysicsVolume(false);
		WheelMesh->SetSimulatePhysics(false);
	}

	//Instance index will be same with body
	int32 CreateInstance(const FTransform& InBodyTransform);

	/*
	//https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1520567-converting-vector-between-local-and-world-space
	FRotator CombineRotators(FRotator RotA, FRotator RotB)
	{
		FQuat AQuat = FQuat(RotA);
		FQuat BQuat = FQuat(RotB);
		return FRotator(BQuat * AQuat);
	}
	FQuat CombineRotators(const FQuat& InRotA, const FQuat& InRotB)
	{
		return InRotB * InRotA;
	}
	*/

	void UpdateInstanceWorldLocation(int32 Index, const FTransform& InBodyLocation, float InSpeed);

private:
	float CurrentRotationValue;
	//FTransform BaseTransform;
	FQuat BaseRotator;
};

UCLASS(BlueprintType, Blueprintable, HideCategories = (Rendering, Replication, Collision, Input, Actor, LOD, Cooking), meta = (BlueprintSpawnableComponent))
class CIVILFXCORE_API AVehicleInstancer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicleInstancer();

	static FTransform GetWorldTransformFromLocal(const FTransform& InParentTransform, const FQuat& InLocalRotator, const FVector& InLocalLocation, const FVector& InLocalScale);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTruck;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVehicleWheelMesh> WheelMeshes;

	UPROPERTY()
	UStaticMeshComponent* DummyMesh;

	//Only needed for truck
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInstancedStaticMeshComponent* TrailerMesh;


#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	bool bShowMeasureTool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVehicleInstancerMeasureTool* ToolComponent;
	FTransform OriginTransform;
#endif

	UPROPERTY(EditAnywhere)
	float FrontOffset;
	UPROPERTY(EditAnywhere)
	float RearOffset;
	UPROPERTY(EditAnywhere)
	float SideOffset;

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	/**
	* Try to Get or Create an instance of the vehicle (including body, wheels, and/or trailer)
	* @return	Index of the created instance
	*/
	int32 GetOrCreateInstance();

	/**
	* Release instance back to the pool
	* @param	Index			Index of the instance
	*/
	void ReleaseInstance(int32 Index);


	//*****//
	//Never remove an instance, set that instance's scale to 0 instead

	/**
	* Update transform of each instance. Also Update the transform of wheels
	* @param	Index				Index of the instance
	* @param	InBodyTransform		World Transform
	* @param	InSpeed				Speed of the vehicle, used to update wheels rotation
	*/
	void UpdateInstanceWorldLocation(int32 Index, const FTransform& InBodyTransform, float InSpeed);

private:
	TArray<int32> UnusedInstances;
	FTransform BaseTransform;
	//Reference to subsystem
	UVehicleInstancerManager* Manager;
	void SetVisibility(int32 Index, bool bVisible);

};
