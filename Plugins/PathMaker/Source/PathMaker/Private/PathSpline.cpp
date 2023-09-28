// Fill out your copyright notice in the Description page of Project Settings.


#include "PathSpline.h"
#include "MovingObjectActor.h"
#include "Human.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"


DEFINE_LOG_CATEGORY(MyLog)

// Sets default values
APathSpline::APathSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(FName(TEXT("Spline")));
	SplineComponent->SetRelativeLocation(FVector::ZeroVector);
	SplineComponent->SetRelativeScale3D(FVector(1));
	// When a spline is created, set the second node to 2000 units
	SplineComponent->SetLocationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1, FVector(2000, 0, 0), ESplineCoordinateSpace::World, true);
	//SplineComponent->bShouldVisualizeScale = true;
	this->SetRootComponent(SplineComponent);

}

// Called when the game starts or when spawned
void APathSpline::BeginPlay()
{
	Super::BeginPlay();
}

void APathSpline::ShouldSpawnAMover()
{


}

void APathSpline::SpawnAMover(TSubclassOf<AActor> moc)
{

}

void APathSpline::OnSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSpawn called"));
	GenerateComponents(SplineComponent);
}

void APathSpline::MovingObjectRespawn()
{
}

void APathSpline::GenerateComponents(USplineComponent* spline)
{
	if (spline == nullptr)
	{
		return;
	}

	FVector locationStart, locationEnd;
	FVector tangentStart, tangentEnd;
	FVector scaleStart, scaleEnd;
	FVector2D scaleStart2D;
	FVector2D scaleEnd2D;

	scaleStart2D = scaleEnd2D = Scale;

	//Clean up mesh components
	if (Components.Num() > 0)
	{
		for (int32 i = 0; i < Components.Num(); ++i)
		{
			if (Components[i])
			{
				Components[i]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
				Components[i]->DestroyComponent();
			}
		}
		Components.Empty();
	}

	if (ShowMesh && SegmentLength > 1)
	{
		float total = spline->GetSplineLength();
		TArray<FVector2D> segments;
		float current = 0;

		while (current < total)
		{
			segments.Add(FVector2D(current, UKismetMathLibrary::FMin(current + SegmentLength, total)));
			current += SegmentLength;
		}

		for (int32 i = 0; i < segments.Num(); ++i)
		{
			USplineMeshComponent* splineMesh = NewObject<USplineMeshComponent>(this);
			splineMesh->RegisterComponent();
			splineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			GetLatTanScaleAtDistance(segments[i].X, locationStart, tangentStart, scaleStart);
			GetLatTanScaleAtDistance(segments[i].Y, locationEnd, tangentEnd, scaleEnd);
			tangentStart = tangentStart.GetClampedToMaxSize(SegmentLength);
			tangentEnd = tangentEnd.GetClampedToMaxSize(SegmentLength);

			splineMesh->SetMobility(EComponentMobility::Type::Movable);
			splineMesh->SetForwardAxis(ForwardAxis);
			splineMesh->SetStaticMesh(Mesh);
			splineMesh->SetMaterial(0, material);
			splineMesh->SetStartAndEnd(locationStart, tangentStart, locationEnd, tangentEnd);
			splineMesh->SetStartScale(FVector2D(scaleStart.Y, scaleStart.Z));
			splineMesh->SetEndScale(FVector2D(scaleEnd.Y, scaleEnd.Z));
			splineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			splineMesh->AttachToComponent(spline, FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			Components.Add(splineMesh);


		}
	}

}

void APathSpline::GetLatTanScaleAtDistance(float distance, FVector& loc, FVector& tan, FVector& scale)
{
	loc = SplineComponent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
	tan = SplineComponent->GetTangentAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
	scale = SplineComponent->GetScaleAtDistanceAlongSpline(distance);
}

void APathSpline::UpdatePosition()
{
}

// Called every frame
void APathSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APathSpline::OnConstruction(const FTransform& transform)
{
	// Check if mesh does not have save object.
	// If it does, delay execution of path mesh spawn.
	if (!this->HasAnyFlags(EObjectFlags::RF_Transient))
	{
		this->GetWorldTimerManager().SetTimer(PathMeshSpawnHandler, this, &APathSpline::OnSpawn, 0.2f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Path maker does not contain RF_Transient Flags"));
	}
}

