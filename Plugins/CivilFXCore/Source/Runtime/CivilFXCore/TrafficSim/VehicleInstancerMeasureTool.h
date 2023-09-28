/*
Dummy Component to just show a line when bShowMeasureTool is true
Used by VehicleInstancer.h
*/


#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "VehicleInstancerMeasureTool.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, HideCategories = (Rendering, Replication, Collision, Input, Actor, LOD, Cooking), meta = (BlueprintSpawnableComponent))
class CIVILFXCORE_API UVehicleInstancerMeasureTool : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	UVehicleInstancerMeasureTool();

	UPROPERTY(EditAnywhere)
	bool bShowMeasureTool;

	FTransform LineStartTransform;
	FTransform LineEndTransform;

#if !UE_BUILD_SHIPPING
	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.


	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	/** Helper function to draw a vector curve */
	static void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View, const UVehicleInstancerMeasureTool* ToolComp, const FMatrix& LocalToWorld, uint8 DepthPriorityGroup);
#endif
};
