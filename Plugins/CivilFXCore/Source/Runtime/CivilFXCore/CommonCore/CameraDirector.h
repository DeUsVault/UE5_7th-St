// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivilFXCore/TrafficSim/TrafficPath.h"
#include "CameraDirector.generated.h"


struct FCameraSplineData
{
	TSharedPtr<FSplineBuilder> Spline;
	FRotator StartRotation;
	FRotator EndRotation;
	float Duration;
	float CurrentDuration;
	TArray<FVector> Locations;

	float StartTimeline;
	float EndTimeline;

	FCameraSplineData(const TArray<FVector>& InLocations, const FRotator& InStartRotation, const FRotator& InEndRotation) :
		StartRotation(InStartRotation),
		EndRotation(InEndRotation),
		Duration(0.f),
		CurrentDuration(0.f),
		Locations(InLocations)
	{
		check(InLocations.Num() == 4);
	}

	FORCEINLINE bool IsDone() const
	{
		return CurrentDuration >= Duration;
	}
	
	FORCEINLINE bool IsWithinTimeline(float Timeline) const
	{
		return Timeline >= StartTimeline && Timeline <= EndTimeline;
	}

	void BuildSplineData()
	{
		check(Locations.Num() == 4);
		Spline = MakeShared<FSplineBuilder>(Locations);
	}

	FORCEINLINE float GetSplineLength() const
	{
		return Spline->GetCurveLength();
	}

	void ResetData()
	{
		CurrentDuration = 0;
	}

	bool GetNextView(FVector& OutLocation, FRotator& OutRotation, float DeltaTime)
	{
		if (!IsDone())
		{
			float LocalProgress = FMath::GetMappedRangeValueClamped(FVector2D(0, Duration), Normalized2D, CurrentDuration);
			//GLog->Log(FString::Printf(TEXT("Duration: {%f}, CurrentDuration: {%f}, Local Progress: {%f}"), Duration, CurrentDuration, LocalProgress));
			GetViewInternal(OutLocation, OutRotation, LocalProgress);
			CurrentDuration += DeltaTime;
			return true;
		}
		return false;
	}

	void GetViewAtTimeline(float Timeline, FVector& OutLocation, FRotator& OutRotation) const
	{
		float LocalProgress = FMath::GetMappedRangeValueClamped(FVector2D(StartTimeline, EndTimeline), Normalized2D, Timeline);
		GetViewInternal(OutLocation, OutRotation, LocalProgress);
	}

	void GetViewInternal(FVector& OutLocation, FRotator& OutRotation, float InProgress) const
	{
		OutLocation = Spline->GetPointOnPath(InProgress);
		OutRotation = FMath::Lerp(StartRotation, EndRotation, InProgress);
	}

	static const FVector2D Normalized2D;
};


UCLASS(MinimalAPI)
class ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();
	virtual void Tick(float DeltaTime) override;

	void OnBeginCameraDirectorMove(const TArray<FVector>& InLocations, const TArray<FRotator>& InRotations, float Duration, float HeightOffset, bool bPreview=false, bool bInLoop=true);
	void MoveCamera(float DeltaTime);
	void StopCamera();
	void ResumeCamera();
	void GetViewAtTimeline(float Timeline, FVector& OutLocation, FRotator& OutRotation) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TArray<FVector> InternalLocations;
	TArray<FRotator> InternalRotations; 
	TArray<FCameraSplineData> CameraDatas;
	int32 CurrentIndex;
	bool bLoop;
};
