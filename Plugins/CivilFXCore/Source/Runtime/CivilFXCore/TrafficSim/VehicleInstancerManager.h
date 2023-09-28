/*
Singleton responsible for instantiate and update all mesh instances
*/

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VehicleInstancerManager.generated.h"

class AVehicleInstancer;

struct FVehicleInstanceID
{
	FVehicleInstanceID() : BucketID(-1), InstancedIndex(-1){}
	FVehicleInstanceID(int32 InBucketID, int32 InInstancedIndex) : BucketID(InBucketID), InstancedIndex(InInstancedIndex){}
	int32 BucketID;
	int32 InstancedIndex;
	bool IsValid() const
	{
		return BucketID != -1 && InstancedIndex != -1;
	}
};

/**
 *
 */
UCLASS()
class CIVILFXCORE_API UVehicleInstancerManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UVehicleInstancerManager();

	void RegisterInstancedActor(AVehicleInstancer* Instancer);

	FVehicleInstanceID CreateVehicle(bool bAllowTruck = true);
	void ReleaseVehicle(FVehicleInstanceID& InOutVehicleID);
	void UpdateTransform(const FVehicleInstanceID& VehicleID, const FTransform& InBodyTransform, float InSpeed);

private:
	TMap<int32, AVehicleInstancer*> Bucket;
	int32 UniqueBucketID;
};
