// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CameraDirector.h"

const FVector2D FCameraSplineData::Normalized2D = FVector2D(0, 1);

// Sets default values
ACameraDirector::ACameraDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.SetTickFunctionEnable(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Component"), true);

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveCamera(DeltaTime);
}

void ACameraDirector::OnBeginCameraDirectorMove(const TArray<FVector>& InLocations, const TArray<FRotator>& InRotations, float Duration, float HeightOffset, bool bPreview/*=false*/, bool bInLoop/*=true*/)
{
	CameraDatas.Empty();
	InternalLocations = InLocations;
	InternalRotations = InRotations;

	for (FVector& Node : InternalLocations)
	{
		Node.Z += HeightOffset;
	}

	int32 ElementCount = InternalLocations.Num();
	for (int32 i = 0; i < ElementCount - 1; ++i)
	{
		FVector ControlStart;
		FVector ControlEnd;
		if (i == 0)
		{
			ControlStart = InternalLocations[0] + (InternalLocations[0] - InternalLocations[1]);
		}
		else
		{
			ControlStart = InternalLocations[i - 1];
		}

		if (i == ElementCount - 2)
		{
			ControlEnd = InternalLocations[ElementCount - 1] + (InternalLocations[ElementCount - 1] - InternalLocations[ElementCount - 2]);
		}
		else
		{
			ControlEnd = InternalLocations[i + 2];
		}

		CameraDatas.Add(FCameraSplineData(
			{ ControlStart, InternalLocations[i], InternalLocations[i + 1], ControlEnd },
			InternalRotations[i], InternalRotations[i + 1]
		));
	}

	ElementCount = CameraDatas.Num();
	float TimelineSegment = FMath::GetMappedRangeValueClamped(FVector2D(0, Duration), FVector2D(0, 1), Duration / ElementCount);
	for (int32 i = 0; i < ElementCount; ++i)
	{
		CameraDatas[i].BuildSplineData();
		CameraDatas[i].Duration = Duration / ElementCount;
		CameraDatas[i].StartTimeline = i * TimelineSegment;
		CameraDatas[i].EndTimeline = (i + 1) * TimelineSegment;
	}

	CurrentIndex = 0;

	/*
	FString DebugStr;
	for (FCameraSplineData& Data : CameraDatas)
	{
		DebugStr.Append(FString::Printf(TEXT("{%f}"), Data.Duration));
	}
	GLog->Log(DebugStr);
	*/

	if (!bPreview)
	{
		SetActorTickEnabled(true);
	}
	bLoop = bInLoop;
}

void ACameraDirector::MoveCamera(float DeltaTime)
{
	if (CurrentIndex >= CameraDatas.Num())
	{
		if (bLoop)
		{
			CurrentIndex = 0;
			for (FCameraSplineData& Data : CameraDatas)
			{
				Data.ResetData();
			}
		}
		else
		{
			//move to final view before we stop the camera
			FVector FinalLocation;
			FRotator FinalRotation;
			GetViewAtTimeline(1.0f, FinalLocation, FinalRotation);
			SetActorLocationAndRotation(FinalLocation, FinalRotation);
			StopCamera();
		}
	}
	else
	{
		FVector Location;
		FRotator Rotation;
		if (CameraDatas[CurrentIndex].GetNextView(Location, Rotation, DeltaTime))
		{
			SetActorLocationAndRotation(Location, Rotation);
		}
		else
		{
			++CurrentIndex;
		}
	}
}

void ACameraDirector::StopCamera()
{
	SetActorTickEnabled(false);
}

void ACameraDirector::GetViewAtTimeline(float Timeline, FVector& OutLocation, FRotator& OutRotation) const
{
	bool bFound = false;
	for (const FCameraSplineData& Data : CameraDatas)
	{
		if (Data.IsWithinTimeline(Timeline))
		{
			Data.GetViewAtTimeline(Timeline, OutLocation, OutRotation);
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		//Handle the extreme case - when Timeline is exactly 1.0
		CameraDatas[CameraDatas.Num() - 1].GetViewAtTimeline(0.9999f, OutLocation, OutRotation);
	}
}

