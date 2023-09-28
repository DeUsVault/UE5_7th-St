// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "ChevronCreator.h"
#include "CivilFXCore/TrafficSim/TrafficPath.h"

#include "Engine/StaticMesh.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

class UStaticMesh* AChevronCreator::MeshToBuild = nullptr;
float AChevronCreator::MeshLength_Meter = 0.f;

// Sets default values
AChevronCreator::AChevronCreator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (MeshToBuild == nullptr)
	{
		MeshToBuild = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/CivilFXCore/Chevron/Chevron_365_SM_Base.Chevron_365_SM_Base'"), nullptr, LOAD_None, nullptr);
		MeshLength_Meter = MeshToBuild->GetBounds().GetBox().GetSize().X / 100.0f;

		GLog->Log(FString::Printf(TEXT("Mesh Length: %f"), MeshLength_Meter));

	}
	check(MeshToBuild);
}

void AChevronCreator::BuildChevronMesh(const TArray<FVector>& InLocations, FLinearColor ChevronColor, int32 ChevronWidthMultiplier/*=1*/, float ChevronSpeed/*=1.5f*/, bool bRetraceLocation/*=true*/, float HeightOffset/*=100.f*/)
{
	const int32 MINIMUM_NODES = 4;
	if (InLocations.Num() < MINIMUM_NODES)
	{
		return;
	}

	SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	SetActorEnableCollision(false);

	Mat = UMaterialInstanceDynamic::Create(MeshToBuild->GetMaterial(0), this);
	Mat->SetScalarParameterValue(TEXT("ChevronSpeed"), ChevronSpeed);
	Mat->SetVectorParameterValue(TEXT("BaseColor"), ChevronColor);
	FLinearColor OutlineColor = FLinearColor(1 - ChevronColor.R, 1 - ChevronColor.G, 1 - ChevronColor.B, ChevronColor.A);
	Mat->SetVectorParameterValue(TEXT("OutlineColor"), OutlineColor);
	//copy the locations;
	TArray<FVector> Locations = InLocations;

	//pad start and end control points
	int32 ElementCount = Locations.Num();
	FVector ControlStart = Locations[0] + (Locations[0] - Locations[1]).GetSafeNormal() * 100;
	FVector ControlEnd = Locations[ElementCount - 1] + (Locations[ElementCount - 1] - Locations[ElementCount - 2]);
	Locations.Add(ControlEnd);
	Locations.Insert(ControlStart, 0);

	for (FVector& Location : Locations)
	{
		Location.Z += HeightOffset;
	}

	TSharedPtr<FSplineBuilder> SplineData = MakeShared<FSplineBuilder>(Locations);
	const float SplineLength = SplineData->GetCurveLength();
	const float SectionLength = MeshLength_Meter * ChevronWidthMultiplier;

	for (float SegmentLength = 0; SegmentLength < SplineLength - SectionLength; SegmentLength += SectionLength)
	{
		float EndSegment = SegmentLength + SectionLength;

		FVector StartLocation = SplineData->GetPointOnPathSegment(SegmentLength);
		FVector StartTangent = SplineData->GetTangentOnPathSegment(SegmentLength);

		FVector EndLocation = SplineData->GetPointOnPathSegment(EndSegment);
		FVector EndTangent = SplineData->GetTangentOnPathSegment(EndSegment);

		FVector2D Scale = FVector2D::UnitVector * ChevronWidthMultiplier;

		USplineMeshComponent* SplineMeshComp = NewObject<USplineMeshComponent>(this);

		SplineMeshComp->SetStaticMesh(MeshToBuild);
		SplineMeshComp->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, false);
		SplineMeshComp->SetMaterial(0, Mat);
		SplineMeshComp->SetStartScale(Scale, false);
		SplineMeshComp->SetEndScale(Scale, true);

		SplineMeshComp->OnComponentCreated();
		if (RootComponent == nullptr)
		{
			RootComponent = SplineMeshComp;
		}
		else
		{
			SplineMeshComp->SetupAttachment(RootComponent);
		}

		if (SplineMeshComp->bAutoRegister)
		{
			SplineMeshComp->RegisterComponent();
		}
	}
	SetActorHiddenInGame(true);
}

