// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficPathController.h"
#include "GameFramework/Actor.h"

#if WITH_EDITOR
#include "ScopedTransaction.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#endif

// Sets default values for this component's properties
UTrafficPathController::UTrafficPathController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTrafficPathController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



//#if !UE_BUILD_SHIPPING
#if WITH_EDITOR
FPrimitiveSceneProxy* UTrafficPathController::CreateSceneProxy()
{
	class FLinesSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FLinesSceneProxy(const UTrafficPathController* InComponent) :
			FPrimitiveSceneProxy(InComponent),
			PathController(InComponent)
		{
			bDrawDebug = InComponent->GetTrafficPath()->bDrawDebug;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_SplineSceneProxy_GetDynamicMeshElements);

			if (IsSelected())
			{
				return;
			}

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					const FMatrix& LocalToWorld = GetLocalToWorld();

					// Taking into account the min and maximum drawing distance
					const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalToWorld.GetOrigin()).SizeSquared();
					if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
					{
						continue;
					}
					UTrafficPathController::Draw(PDI, View, PathController);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = bDrawDebug && !IsSelected() && IsShown(View) && View->Family->EngineShowFlags.Splines;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
		uint32 GetAllocatedSize(void) const { return FPrimitiveSceneProxy::GetAllocatedSize(); }
	private:
		const UTrafficPathController* PathController;
		bool bDrawDebug;
	};

	return new FLinesSceneProxy(this);
}

FBoxSphereBounds UTrafficPathController::CalcBounds(const FTransform& LocalToWorld) const
{
	if (GetTrafficPath() == nullptr)
	{
		return GetOwner()->GetComponentsBoundingBox();
	}
	return GetTrafficPath()->CalcBounds(LocalToWorld);
}

void UTrafficPathController::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View, const UTrafficPathController* PathController)
{
	TArray<int32> XMarkedLanes;
	int32 LanesCount = PathController->GetTrafficPath()->GetLanesCount();
	float PathLength = PathController->GetTrafficPath()->GetPathLength();

	TSharedPtr<FSplineBuilder> Spline = PathController->GetTrafficPath()->GetSplineBuilder();

	for (int32 i = 0; i < PathController->GetTrafficPath()->GetLanesCount(); ++i)
	{
		XMarkedLanes.Add(i);
	}

	const TArray<FRamp>& Ramps = PathController->GetRamps();
	for (const FRamp& Ramp : Ramps)
	{
		if (Ramp.NewPathController == nullptr)
		{
			continue;
		}

		for (int32 Lane : Ramp.FromLanes)
		{
			FVector Position = PathController->GetWorldPositionFromLaneIndexEditor(Ramp.CurvePosMax, Lane);
			FVector Direction = Spline->GetTangentOnPathSegment(Ramp.CurvePosMax);
			FVector LeftDir = FVector::CrossProduct(Direction, FVector::UpVector);

			const float MainShaftHalfLength = 80.f;
			const float ShortSaftHalfLength = MainShaftHalfLength / 2.f;
			const float ArrowHeadVerticalSpace = 30.f;
			const float ArrowHeadHorizontalSpace = ArrowHeadVerticalSpace;

			PDI->DrawPoint(Position, Ramp.Color, 6.0f, SDPG_Foreground);

			//draw merge/diver mark
			/*
				P0                 P0
			     .                  .
			  P1/|\P2			 P1/|\P2P4_P3
				 |P3_P5				|     /|
				 ||\				|   /  P5
				 |P4\				|  P7
				 P6 P7              P6           
			*/
			FVector P0 = Position + Direction * MainShaftHalfLength;
			FVector P6 = Position - Direction * MainShaftHalfLength;
			FVector P1 = (P0 - Direction * ArrowHeadVerticalSpace) + (LeftDir * ArrowHeadHorizontalSpace);
			FVector P2 = (P0 - Direction * ArrowHeadVerticalSpace) - (LeftDir * ArrowHeadHorizontalSpace);
			FVector P3, P4, P5, P7;
			if (Ramp.Type == ERampType::Merge)
			{
				P3 = Position + Direction * MainShaftHalfLength / 2;
				P7 = (Position - Direction * MainShaftHalfLength / 3) - (LeftDir * MainShaftHalfLength / 1.5);
			}
			else if (Ramp.Type == ERampType::Diverge)
			{
				P7 = Position - Direction * MainShaftHalfLength / 2;
				P3 = (Position + Direction * MainShaftHalfLength / 3) - (LeftDir * MainShaftHalfLength / 1.5f);
			}
			FVector ShortShaftDirection = (P3 - P7).GetSafeNormal();
			FVector ShortShaftLeftDirection = FVector::CrossProduct(ShortShaftDirection, FVector::UpVector);
			P4 = (P3 - ShortShaftDirection * MainShaftHalfLength / 4) + ShortShaftLeftDirection * ArrowHeadHorizontalSpace / 3;
			P5 = (P3 - ShortShaftDirection * MainShaftHalfLength / 4) - ShortShaftLeftDirection * ArrowHeadHorizontalSpace / 3;

			PDI->DrawLine(P0, P6, Ramp.Color, SDPG_Foreground);
			PDI->DrawLine(P0, P1, Ramp.Color, SDPG_Foreground);
			PDI->DrawLine(P0, P2, Ramp.Color, SDPG_Foreground);
			PDI->DrawLine(P3, P7, Ramp.Color, SDPG_Foreground);
			PDI->DrawLine(P3, P4, Ramp.Color, SDPG_Foreground);
			PDI->DrawLine(P3, P5, Ramp.Color, SDPG_Foreground);

			XMarkedLanes.Remove(Lane);
		}
	}


	//draw X mark for lane that does not have a merge
	for (int32 XMarkedLane : XMarkedLanes)
	{
		FVector Position = PathController->GetWorldPositionFromLaneIndexEditor(PathLength, XMarkedLane);
		FVector Direction = Spline->GetTangentOnPathSegment(PathLength);

		/*
			P0	P1	
			 \  /
			  \/
			  /\
			 /  \
			P2  P3
		*/
		float HorizontalHalfLength = 100.f;
		float VerticalHalfLength = 100.f;

		FVector TopWidth = Direction * VerticalHalfLength;
		FVector LeftWidth = FVector::CrossProduct(Direction, FVector::UpVector) * HorizontalHalfLength;

		FVector P0 = Position + TopWidth + LeftWidth;
		FVector P1 = Position + TopWidth - LeftWidth;
		FVector P2 = Position - TopWidth + LeftWidth;
		FVector P3 = Position - TopWidth - LeftWidth;

		PDI->DrawLine(P0, P3, FLinearColor::Red, SDPG_Foreground);
		PDI->DrawLine(P1, P2, FLinearColor::Red, SDPG_Foreground);

	}

}

#endif


#if WITH_EDITOR
void UTrafficPathController::OnComponentCreated()
{
	Super::OnComponentCreated();

	const FScopedTransaction Transaction(FText::FromString(TEXT("Getting reference to TrafficPath")));

	AActor* Owner = GetOwner();
	Owner->Modify();
	this->Modify();
	UTrafficPath* CurrentTrafficPath = Cast<UTrafficPath>(Owner->GetComponentByClass(UTrafficPath::StaticClass()));
	if (CurrentTrafficPath)
	{
		//if the TrafficPath component already existed, set it
		this->TrafficPath = CurrentTrafficPath;
	}
	else
	{
		//create new TrafficPath component and add to the actor
		UTrafficPath* NewTrafficPath = NewObject<UTrafficPath>(Owner, TEXT("Traffic Path"));
		Owner->AddInstanceComponent(NewTrafficPath);
		NewTrafficPath->SetupAttachment(Owner->GetRootComponent());
		NewTrafficPath->RegisterComponent();

		this->AttachToComponent(NewTrafficPath, FAttachmentTransformRules::KeepRelativeTransform);
		this->TrafficPath = NewTrafficPath;

		static const FName LevelEditorModuleName("LevelEditor");
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked< FLevelEditorModule >(LevelEditorModuleName);
		TArray<UObject*> SelectedObjects;
		SelectedObjects.Add(Owner);
		LevelEditorModule.BroadcastActorSelectionChanged(SelectedObjects, false);
	}

	//clear some data incase this component is copied (via actor copied)
	Ramps.Empty();

}
UTrafficPath* UTrafficPathController::GetTrafficPath()
{
	//check(TrafficPath);
	return TrafficPath;
	//UTrafficPath* Result = Cast<UTrafficPath>(GetOwner()->GetComponentByClass(UTrafficPath::StaticClass()));
	//return Result;
}
UTrafficPath* UTrafficPathController::GetTrafficPath() const
{
	//check(TrafficPath);
	return TrafficPath;
	//UTrafficPath* Result = Cast<UTrafficPath>(GetOwner()->GetComponentByClass(UTrafficPath::StaticClass()));
	//return Result;
}

const TArray<FRamp>& UTrafficPathController::GetRamps() const
{
	return Ramps;
}

FVector UTrafficPathController::GetWorldPositionFromLaneIndexEditor(float CurvePos, int32 Lane) const
{
	TSharedPtr<FSplineBuilder> PathSpline = GetTrafficPath()->GetSplineBuilder();
	int32 LanesCount = GetTrafficPath()->GetLanesCount();
	float HalfPathWidth = GetTrafficPath()->GetPathWidth() / 2.f;
	FVector Center = PathSpline->GetPointOnPathSegment(CurvePos);
	FVector Direction = PathSpline->GetTangentOnPathSegment(CurvePos);
	FVector LeftWidth = FVector::CrossProduct(Direction, FVector::UpVector) * HalfPathWidth;
	float Segment = (2.f * Lane + 1.f) / (2.f * LanesCount);
	return FMath::Lerp(Center + LeftWidth, Center - LeftWidth, Segment);
}
#endif