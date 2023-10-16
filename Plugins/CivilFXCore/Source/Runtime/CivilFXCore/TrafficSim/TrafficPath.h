// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "TrafficPath.generated.h"

struct CIVILFXCORE_API FSplineBuilder
{

public:
	FSplineBuilder(const TArray<FVector>& InNodes);
	FORCEINLINE int32 GetNodesCount() const
	{
		return Nodes.Num();
	}

	/**	Return the length of path (in meters)
	* 
	*/
	FORCEINLINE float GetCurveLength() const
	{
		return CurveLength;
	}

	/**	Get Rough World Point On Path
	*	Use this function to draw the path in the editor
	*	@Params:
	*		Time: The current progress along the path [0,1]
	*/
	FVector GetPoint(float Time) const;

	/**	Get Rough World Direction (normalized) On Path
	*	Use this function to draw the path in the editor
	*	@Params:
	*		Time: The current progress along the path [0,1]
	*/
	FVector GetTangent(float Time) const;


	/**	Get Smooth World Point On Path
	*	Use this function for gameplay
	*	@Params:
	*		Time: The current progress along the path [0,1]
	*/
	FVector GetPointOnPath(float Time) const;

	/**	Get Smooth World Point On Path
	*	Use this function for gameplay
	*	@Params:
	*		Segment: The current curve segment [0,CurveLength]
	*/
	FVector GetPointOnPathSegment(float Segment) const;
	float GetProgressOnPath(FVector Position) const;

	FVector GetTangentOnPath(float Time) const;
	FVector GetTangentOnPathSegment(float Segment) const;
	FRotator GetRotation(float Time) const;
	static FRotator LookRotation(const FVector& Forward, const FVector& Up);

private:
	const TArray<FVector>& Nodes;

	struct FSplineSegment
	{
		float Time;
		float Distance;
		FVector Position;
	};
	//internal equalize curve segments
	//to cache the path for faster lookup
	TArray<FSplineSegment> Segments;

	//overall curve length of path
	float CurveLength;

	//how big is the cache
	static CONSTEXPR int32 SEGMENT_RESOLUTION = 5;

	void BuildPath();
	float GetSmoothTimeOnCurve(float Time) const;

};


/**
 * 
 */
UENUM()
enum class EPathType
{
	Urban,
	Freeway
};

UCLASS(ClassGroup = Custom, ShowCategories = (Mobility), HideCategories = (Physics, Collision, Lighting, Rendering, Mobile, VirtualTexture, Tags, Activation, HLOD, Cooking, AssetUserData), meta = (BlueprintSpawnableComponent))
class CIVILFXCORE_API UTrafficPath : public UPrimitiveComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this component's properties
	UTrafficPath();
	static CONSTEXPR uint8 MIN_NODES_COUNT = 4;
	static CONSTEXPR float NODE_DISTANCE = 3000.f;
public:

	TSharedPtr<FSplineBuilder> GetSplineBuilder(bool ForceRebuild = false);

	FORCEINLINE TArray<FVector>& GetNodes()
	{
		return Nodes;
	}

	FORCEINLINE const TArray<FVector>& GetNodes() const
	{
		return Nodes;
	}

	FORCEINLINE const FLinearColor& GetSplineColor() const
	{
		return SplineColor;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetLanesCount() const
	{
		return LanesCount;
	}

	FORCEINLINE int32 GetSplineResolution() const
	{
		return SplineResolution;
	}

	FORCEINLINE float GetLaneWidth() const
	{
		return WidthPerLane;
	}
	
	FORCEINLINE float GetPathWidth() const
	{
		return WidthPerLane * LanesCount;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetPathLength()
	{
		return GetSplineBuilder()->GetCurveLength();
	}

	/** Added a copy of this function here to be used in blueprint */
	/** Needed for mesh building actor(s) */
	UFUNCTION(BlueprintCallable)
	FVector GetWorldPositionFromLaneIndex(float CurvePos, int32 Lane) const;
	UFUNCTION(BlueprintCallable)
	FVector GetWorldTangent(float CurvePos) const;

#if !UE_BUILD_SHIPPING
	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.


	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	/** Helper function to draw a vector curve */
	static void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View, const UTrafficPath* TrafficPath, bool bDrawHandles=true);
#endif

#if WITH_EDITOR
	void UpdateNode(int32 Index, FVector Location);
	void UpdateNodes(FVector DeltaLocation);
	void InsertNode(int32 Index, FVector Location);
	void ReverseNodes();
#endif


protected:

	virtual void OnComponentCreated() override;

private:
	UPROPERTY(EditAnywhere, Category = "Path Settings")
	float WidthPerLane = 365.76f; //12ft : 3.6576m

	UPROPERTY(EditAnywhere, Category = "Path Settings")
	uint8 LanesCount = 1;

	UPROPERTY(EditAnywhere, Category = "Path Settings")
	uint16 SplineResolution = 300;

	UPROPERTY(EditAnywhere, Category = "Path Settings")
	FLinearColor SplineColor = FColor::Green;



	UPROPERTY(EditAnywhere, Category = "Path Settings")
	TArray<FVector> Nodes;

	TSharedPtr<FSplineBuilder> SplineBuilder;

public:
	UPROPERTY(EditAnywhere, Category = "Path Settings")
	EPathType PathType;

	UPROPERTY(EditAnywhere, Category = "Path Settings")
	bool bDrawDebug = true;
};
