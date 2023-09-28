/*
 * This class creates the spline path usable to any
 * object running through this path. This class can
 * also host a spline mesh so any solid mesh can be used
 * to represent roads.
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PathSpline.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MyLog, Log, All);

UCLASS()
class PATHMAKER_API APathSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathSpline();

	UPROPERTY(EditAnywhere, Category = "Path Maker")
		USplineComponent* SplineComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool bPathIsLooping = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnSpawn();
	UFUNCTION()
		void MovingObjectRespawn();
	UFUNCTION()
		void GenerateComponents(USplineComponent* spline);
	UFUNCTION()
		void GetLatTanScaleAtDistance(float distance, FVector& loc, FVector& tan, FVector& scale);
	UFUNCTION()
		void UpdatePosition();
	UFUNCTION()
		void SpawnAMover(TSubclassOf<AActor> moc);
	UFUNCTION()
		void ShouldSpawnAMover();

	UPROPERTY(EditAnywhere, Category = "Path Maker")
		TArray<USplineMeshComponent*> Components;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool ShowMesh = false;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float SegmentLength = 100;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		FVector2D Scale = FVector2D(1, 1);
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		UStaticMesh* Mesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		UMaterial* material = nullptr;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		TArray<TSubclassOf<AActor>> MovingObjectClass;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		float SecondsToCompletePath = 8.0f;
	UPROPERTY(EditAnywhere, Category = "Path Maker")
		bool bPauseInBetweenLoops = true;
	//UPROPERTY(EditAnywhere, Category = "Path Maker")
	//	float PuaseTimeInSeconds = 5.0f;
	//UPROPERTY(EditAnywhere, Category = "Path Maker")
	//	int MaxNumOfPeople = TOTAL_NUM_OF_PED;

	//TArray<AMovingObjectActor*> MovingObject;
	//TArray<AHuman*> MovingObject;
	//float StartTime = 0.0f;
	//bool bCanMoveObject = false;
	//float CurrentSplineTime = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& transform) override;

private:

	//static const int TOTAL_NUM_OF_PED = 10;
	//int CurrentMoverIndex;
	//
	FTimerHandle PathMeshSpawnHandler;
	//FTimerHandle MovingObjectRespawnHandler;
	//FTimerHandle UpdatePositionHandler;
	//FTimerHandle ShouldSpawnMoverHandler;

};
