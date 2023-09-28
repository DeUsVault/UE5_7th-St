// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "TrafficPath.h"
#include "TrafficPathController.generated.h"

class UTrafficPath;
class UTrafficPathController;
class AActor;

UENUM()
enum class ERampMode : uint8 { Auto = 0, Manual };

UENUM()
enum class ERampType : uint8 { Merge = 0, Diverge };

UENUM()
enum class ERampDirection :uint8 { ToRight = 0, ToLeft };

USTRUCT()
struct FRamp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	//AActor* Actor;
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY(EditAnywhere)
	//UTrafficPathController* NewPathController;
	TWeakObjectPtr<UTrafficPathController> NewPathController;

	//#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	FLinearColor Color = FLinearColor::Green;
	/*#endif*/

	UPROPERTY(EditAnywhere)
	ERampType Type = ERampType::Merge;

	UPROPERTY(EditAnywhere)
	ERampMode Mode = ERampMode::Auto;

	UPROPERTY(EditAnywhere)
	ERampDirection Direction = ERampDirection::ToLeft;

	UPROPERTY(EditAnywhere)
	float CurvePosOnNewPath = 0.0f;
	UPROPERTY(EditAnywhere)
	float CurvePosMin = 0.0f;
	UPROPERTY(EditAnywhere)
	float CurvePosMax = 0.0f;

	UPROPERTY(EditAnywhere)
	TArray<uint8> FromLanes;
	UPROPERTY(EditAnywhere)
	TArray<uint8> ToLanes;
};

UCLASS(ClassGroup = Custom, ShowCategories = (Mobility), HideCategories = (Physics, Collision, Lighting, Rendering, Mobile, VirtualTexture, Tags, Activation, HLOD, Cooking, AssetUserData), meta = (BlueprintSpawnableComponent))
class CIVILFXCORE_API UTrafficPathController : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrafficPathController();

//#if !UE_BUILD_SHIPPING
#if WITH_EDITOR
	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.


	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	/** Helper function to draw a vector curve */
	static void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View, const UTrafficPathController* TrafficPath);
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


protected:
	UPROPERTY(EditAnywhere)
	TArray<FRamp> Ramps;
	UPROPERTY()
	UTrafficPath* TrafficPath;

#if WITH_EDITOR
protected:
	virtual void OnComponentCreated() override;
public:
	UTrafficPath* GetTrafficPath();
	UTrafficPath* GetTrafficPath() const;
	const TArray<FRamp>& GetRamps() const;
	FVector GetWorldPositionFromLaneIndexEditor(float CurvePos, int32 Lane) const;
#endif

private:
	TSharedPtr<FSplineBuilder> Spline;

};
