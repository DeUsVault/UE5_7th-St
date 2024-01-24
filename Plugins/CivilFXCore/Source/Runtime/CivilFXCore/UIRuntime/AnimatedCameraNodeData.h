// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "AnimatedCameraNodeData.generated.h"

USTRUCT()
struct FAnimatedCameraNodeData
{
	GENERATED_BODY()

	FAnimatedCameraNodeData();
	UPROPERTY()
	FString CameraName;
	UPROPERTY()
	FString CameraType;
	UPROPERTY()
	float Duration;
	UPROPERTY()
	FString Category;
	UPROPERTY()
	TArray<FString> Phases;

	/**  */
	UPROPERTY()
	FVector CenterLocation;
	UPROPERTY()
	float Height;
	UPROPERTY()
	float XRadius;
	UPROPERTY()
	float YRadius;
	UPROPERTY()
	int32 Size;
	/**  */
	UPROPERTY()
	TArray<FVector> Locations;
	UPROPERTY()
	TArray<FRotator> Rotations;

	bool IsMovementType() const
	{
		return CameraType.Equals(TEXT("Movement"));
	}

	FString GetSerializedString(int32 Indent) const;
	bool SerializeDataFromString(TArray<FString>& InOutLines);

	/** Unserialized data */
	bool bHasLoadedData;

	FString ToString() const;

	friend bool operator==(const FAnimatedCameraNodeData& InLHS, const FAnimatedCameraNodeData& InRHS);
	friend bool operator!=(const FAnimatedCameraNodeData& InLHS, const FAnimatedCameraNodeData& InRHS);
};

//Helper structs to get the id
USTRUCT()
struct FCameraNodeData_ID
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id = -1;

	friend uint32 GetTypeHash(const FCameraNodeData_ID& InData)
	{
		return InData.Id;
	}

	friend bool operator==(const FCameraNodeData_ID& InLHS, const FCameraNodeData_ID& InRHS)
	{
		return InLHS.Id == InRHS.Id;
	}
};



USTRUCT()
struct FNavigationCameraData_ID
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCameraNodeData_ID> AnimatedCamera;
	UPROPERTY()
	TArray<FCameraNodeData_ID> StillCameras;
};
//