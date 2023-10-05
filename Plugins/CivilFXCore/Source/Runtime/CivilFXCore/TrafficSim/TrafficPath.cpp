// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficPath.h"
#include "Components/ActorComponent.h"
#include "PrimitiveSceneProxy.h" //FPrimitiveSceneProxy
#include "Kismet/KismetMathLibrary.h"
#if WITH_EDITOR
#include "ScopedTransaction.h"
#include "Algo/Reverse.h"
#endif

/* FSplineBuilder */
#pragma region FSplineBuild_Implementation

FSplineBuilder::FSplineBuilder(const TArray<FVector>& InNodes) :
	Nodes(InNodes),
	CurveLength(0)
{
	BuildPath();
}

void FSplineBuilder::BuildPath()
{
	float TotalSubdivisions = Nodes.Num() * SEGMENT_RESOLUTION;
	float SegmentLength = 1.0f / TotalSubdivisions;
	FVector LastPoint = GetPoint(0);

	for (int i = 1; i < TotalSubdivisions + 1; i++)
	{

		float CurrentSegment = SegmentLength * i;
		FVector CurrentPoint = GetPoint(CurrentSegment);
		//Convert the distance to meters before adding it
		CurveLength += (FVector::Distance(CurrentPoint, LastPoint) / 100.f);
		LastPoint = CurrentPoint;
		Segments.Add({ CurrentSegment, CurveLength, CurrentPoint });
	}

	/*
	Debug printing
	*/
	return;
	FString DebugString;
	for (FSplineSegment& Segment : Segments)
	{
		DebugString.Append(FString::Printf(TEXT("[%.5f - %.5f]"), Segment.Time, Segment.Distance));
	}
	GLog->Log(DebugString);
}

float FSplineBuilder::GetSmoothTimeOnCurve(float Time) const
{
	float TargetDistance = Time * CurveLength;
	int SegmentCount = Segments.Num();
	int NextSegmentIndex;
	for (NextSegmentIndex = 0; NextSegmentIndex < SegmentCount; NextSegmentIndex++)
	{
		if (Segments[NextSegmentIndex].Distance >= TargetDistance)
		{
			break;
		}
	}
#if (UE_EDITOR)
	NextSegmentIndex = FMath::Clamp(NextSegmentIndex, 0, SegmentCount - 1);
#endif

	FSplineSegment NextSegment = Segments[NextSegmentIndex];

	if (NextSegmentIndex == 0)
	{
		Time = (TargetDistance / NextSegment.Distance) * NextSegment.Time;
	}
	else
	{
		FSplineSegment PreviousSegment = Segments[NextSegmentIndex - 1];
		float SegmentTime = NextSegment.Time - PreviousSegment.Time;
		float SegmentDistance = NextSegment.Distance - PreviousSegment.Distance;
		Time = PreviousSegment.Time + ((TargetDistance - PreviousSegment.Distance) / SegmentDistance) * SegmentTime;
	}
	return Time;
}

FVector FSplineBuilder::GetPoint(float Time) const
{
	//Added to handle invalid Time
	//such as when the curve length ~ 0
	//TODO: exit earlier?
	if (isnan(Time) || isinf(Time))
	{
		return Nodes[1];
	}

	int NumSections = Nodes.Num() - 3;
	int CurrentNode = FMath::Min(FMath::FloorToInt(Time * NumSections), NumSections - 1);
	float U = Time * NumSections - CurrentNode;

	FVector a = Nodes[CurrentNode];
	FVector b = Nodes[CurrentNode + 1];
	FVector c = Nodes[CurrentNode + 2];
	FVector d = Nodes[CurrentNode + 3];

	return .5f *
		(
			(-a + 3.0f * b - 3.0f * c + d) * (U * U * U)
			+ (2.0f * a - 5.0f * b + 4.0f * c - d) * (U * U)
			+ (-a + c) * U
			+ 2.0f * b
			);
}

FVector FSplineBuilder::GetPointOnPath(float Time) const
{
	Time = GetSmoothTimeOnCurve(Time);
	return GetPoint(Time);
}


FVector FSplineBuilder::GetPointOnPathSegment(float Segment) const
{
	float Time = FMath::Clamp(Segment / CurveLength, 0.0f, 1.0f);
	return GetPointOnPath(Time);
}

float FSplineBuilder::GetProgressOnPath(FVector Position) const
{
	//locate nearest segment
	int32 CurrentIndex;
	float Value;
	TArray<float> Distances;
	for (int32 i = 0; i < Segments.Num(); ++i)
	{
		Distances.Add(FVector::DistSquared(Position, Segments[i].Position));
	}
	UKismetMathLibrary::MinOfFloatArray(Distances, CurrentIndex, Value);
	if (CurrentIndex != 0 && CurrentIndex != Segments.Num() - 1)
	{
		float PrevDistance = FVector::DistSquared(Position, Segments[CurrentIndex - 1].Position);
		float NextDistance = FVector::DistSquared(Position, Segments[CurrentIndex + 1].Position);
		if (PrevDistance < NextDistance)
		{
			--CurrentIndex;
		}
	}
	return 0;
}

FVector FSplineBuilder::GetTangent(float Time) const
{
	int32 NumSections = Nodes.Num() - 3;
	int32 CurrentNodeIndex = FMath::Min(FMath::FloorToInt(Time * NumSections), NumSections - 1);
	float U = Time * (float)NumSections - (float)CurrentNodeIndex;

	FVector A = Nodes[CurrentNodeIndex];
	FVector B = Nodes[CurrentNodeIndex + 1];
	FVector C = Nodes[CurrentNodeIndex + 2];
	FVector D = Nodes[CurrentNodeIndex + 3];

	FVector Result = 0.5f * ((-A + C) + 2.0f * (2.0f * A - 5.0f * B + 4.0f * C - D) * U +
		3 * (-A + 3 * B - 3 * C + D) * U * U);

	return Result.GetSafeNormal();
}

FVector FSplineBuilder::GetTangentOnPath(float Time) const
{
	Time = GetSmoothTimeOnCurve(Time);
	return GetTangent(Time);
}

FVector FSplineBuilder::GetTangentOnPathSegment(float Segment) const
{
	float Time = FMath::Clamp(Segment / CurveLength, 0.0f, 1.0f);
	return GetTangentOnPath(Time);
}

//Alternative: https://www.gamedev.net/forums/topic/613595-quaternion-lookrotationlookat-up/
FRotator FSplineBuilder::LookRotation(const FVector& Forward, const FVector& Up)
{
	//https://forums.unrealengine.com/development-discussion/c-gameplay-programming/1482788-posting-the-source-code-for-lookrotation-for-those-who-need-it
	FVector NewUp = Up - (Forward * FVector::DotProduct(Up, Forward));
	NewUp = NewUp.GetSafeNormal();

	FVector Vector = Forward.GetSafeNormal();
	FVector Vector2 = FVector::CrossProduct(NewUp, Vector);
	FVector Vector3 = FVector::CrossProduct(Vector, Vector2);
	float M00 = Vector2.X;
	float M01 = Vector2.Y;
	float M02 = Vector2.Z;
	float M10 = Vector3.X;
	float M11 = Vector3.Y;
	float M12 = Vector3.Z;
	float M20 = Vector.X;
	float M21 = Vector.Y;
	float M22 = Vector.Z;


	float Num8 = (M00 + M11) + M22;
	FQuat CurrentQuat = FQuat();
	if (Num8 > 0.0f)
	{
		float Num = (float)FMath::Sqrt(Num8 + 1.0f);
		CurrentQuat.W = Num * 0.5f;
		Num = 0.5f / Num;
		CurrentQuat.X = (M12 - M21) * Num;
		CurrentQuat.Y = (M20 - M02) * Num;
		CurrentQuat.Z = (M01 - M10) * Num;
		return FRotator(CurrentQuat);
	}
	if ((M00 >= M11) && (M00 >= M22))
	{
		float Num7 = (float)FMath::Sqrt(((1.0f + M00) - M11) - M22);
		float Num4 = 0.5f / Num7;
		CurrentQuat.X = 0.5f * Num7;
		CurrentQuat.Y = (M01 + M10) * Num4;
		CurrentQuat.Z = (M02 + M20) * Num4;
		CurrentQuat.W = (M12 - M21) * Num4;
		return FRotator(CurrentQuat);
	}
	if (M11 > M22)
	{
		float Num6 = (float)FMath::Sqrt(((1.0f + M11) - M00) - M22);
		float Num3 = 0.5f / Num6;
		CurrentQuat.X = (M10 + M01) * Num3;
		CurrentQuat.Y = 0.5f * Num6;
		CurrentQuat.Z = (M21 + M12) * Num3;
		CurrentQuat.W = (M20 - M02) * Num3;
		return FRotator(CurrentQuat);
	}
	float Num5 = (float)FMath::Sqrt(((1.0f + M22) - M00) - M11);
	float Num2 = 0.5f / Num5;
	CurrentQuat.X = (M20 + M02) * Num2;
	CurrentQuat.Y = (M21 + M12) * Num2;
	CurrentQuat.Z = 0.5f * Num5;
	CurrentQuat.W = (M01 - M10) * Num2;

	return FRotator(CurrentQuat);
}

FRotator FSplineBuilder::GetRotation(float Time) const
{
	Time = GetSmoothTimeOnCurve(Time);
	FVector Tangent = GetTangent(Time).GetSafeNormal();
	return LookRotation(Tangent, FVector::UpVector);
}
#pragma endregion
/* End FSplineBuilder */

/* UTrafficPath */
#pragma region UTrafficPath_Implementation

UTrafficPath::UTrafficPath()
{

}

void UTrafficPath::OnComponentCreated()
{
	Super::OnComponentCreated();

	//nodes need to be at least 4 element always
	Modify();
	if (Nodes.Num() < MIN_NODES_COUNT)
	{
		Nodes.Empty();
		FVector DefaultLocation = GetOwner()->GetActorLocation();
		float DefaultRange = NODE_DISTANCE;
		FVector StepRange(DefaultRange, DefaultRange, 0);

		for (int i = 0; i < 4; i++)
		{
			Nodes.Add(DefaultLocation + StepRange * i);
		}
	}
	MarkRenderStateDirty();

}

TSharedPtr<FSplineBuilder> UTrafficPath::GetSplineBuilder(bool bForceRebuild)
{
	if (!SplineBuilder.IsValid() || bForceRebuild)
	{
		SplineBuilder = MakeShared<FSplineBuilder>(Nodes);
	}
	return SplineBuilder;
}

FVector UTrafficPath::GetWorldPositionFromLaneIndex(float CurvePos, int32 Lane) const
{
	float HalfPathWidth = GetPathWidth() / 2.f;
	FVector Center = SplineBuilder->GetPointOnPathSegment(CurvePos);
	FVector Direction = SplineBuilder->GetTangentOnPathSegment(CurvePos);
	FVector LeftWidth = FVector::CrossProduct(Direction, FVector::UpVector) * HalfPathWidth;
	float Segment = (2.f * Lane + 1.f) / (2.f * LanesCount);
	return FMath::Lerp(Center + LeftWidth, Center - LeftWidth, Segment);
}

FVector UTrafficPath::GetWorldTangent(float CurvePos) const
{
	return SplineBuilder->GetTangentOnPathSegment(CurvePos);
}

#if WITH_EDITOR
void UTrafficPath::ReverseNodes()
{
#define LOCTEXT_NAMESPACE "TP"
	const FScopedTransaction Transaction(LOCTEXT("ReverseNodes", "Reversing All Nodes"));
	Modify();
	Algo::Reverse(Nodes);
	MarkRenderStateDirty();
#undef LOCTEXT_NAMESPACE
}
#endif


#if !UE_BUILD_SHIPPING
FPrimitiveSceneProxy* UTrafficPath::CreateSceneProxy()
{
	if (!bDrawDebug)
	{
		return Super::CreateSceneProxy();
	}

	class FSplineSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FSplineSceneProxy(const UTrafficPath* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, TrafficPath(InComponent)
#if WITH_EDITORONLY_DATA
			, LineColor(InComponent->GetSplineColor())
#else
			, LineColor(FLinearColor::White)
#endif
		{
			bDrawDebug = InComponent->bDrawDebug;
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
					UTrafficPath::Draw(PDI, View, TrafficPath);
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
		const UTrafficPath* TrafficPath;
		TSharedPtr<FSplineBuilder> SplineBuilder;
		FLinearColor LineColor;
		bool bDrawDebug;
	};

	return new FSplineSceneProxy(this);
}

void UTrafficPath::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View, const UTrafficPath* TrafficPath, bool bDrawHandles /*=true*/)
{

	//if the actor is hidden due to phasing
	//don't draw
	if (TrafficPath->GetOwner()->IsHidden())
	{
		return;
	}



	//cast away constness to get splinebuilder
	UTrafficPath* TrafficPathComp = const_cast<UTrafficPath*>(TrafficPath);

	const TArray<FVector>& Nodes = TrafficPath->GetNodes();

	if (Nodes.Num() < UTrafficPath::MIN_NODES_COUNT)
	{
		return;
	}

	const TSharedPtr<FSplineBuilder> SplineBuilder = TrafficPathComp->GetSplineBuilder();
	const FLinearColor& Color = TrafficPath->GetSplineColor();
	int32 GrabHandleSize = 10;

	//draw first and last control points
	PDI->DrawLine(Nodes[0], Nodes[1], FLinearColor::Gray, SDPG_Foreground);
	PDI->DrawLine(Nodes[Nodes.Num() - 1], Nodes[Nodes.Num() - 2], FLinearColor::Gray, SDPG_Foreground);

	//draw paths
	float Segmentation = 1.f / TrafficPath->GetSplineResolution();
	float Time;
	int32 LanesCount = TrafficPath->GetLanesCount();
	float PathWidth = TrafficPath->GetPathWidth();
	float LaneWidth = TrafficPath->GetLaneWidth();

	FVector CenterStart = SplineBuilder->GetPoint(0);
	FVector CenterEnd;
	FVector Direction = SplineBuilder->GetTangent(0);
	FVector LeftDir = FVector::CrossProduct(Direction, FVector::UpVector);
	FVector LeftWidth = LeftDir * (PathWidth / 2.0f);

	//draw start line
	PDI->DrawLine(CenterStart + LeftWidth, CenterStart - LeftWidth, Color, SDPG_Foreground);


	for (Time = Segmentation; Time < 1.0f; Time += Segmentation)
	{
		CenterEnd = SplineBuilder->GetPoint(Time);
		PDI->DrawLine(CenterStart + LeftWidth, CenterEnd + LeftWidth, Color, SDPG_Foreground);
		PDI->DrawLine(CenterStart - LeftWidth, CenterEnd - LeftWidth, Color, SDPG_Foreground);

		FVector OldLeftDir = LeftDir;

		Direction = SplineBuilder->GetTangent(Time);
		LeftDir = FVector::CrossProduct(Direction, FVector::UpVector);
		LeftWidth = LeftDir * (PathWidth / 2.0f);

		//draw divider lines
		float LaneSegment = 1.0f / LanesCount;
		float LaneTime = LaneSegment;
		while (LaneTime < 1.0f)
		{
			FVector LaneStart = FMath::Lerp(CenterStart + LeftWidth, CenterStart - LeftWidth, LaneTime);
			FVector LaneEnd = FMath::Lerp(CenterEnd + LeftWidth, CenterEnd - LeftWidth, LaneTime);
			PDI->DrawLine(LaneStart, LaneEnd, Color, SDPG_Foreground);
			LaneTime += LaneSegment;
		}
		CenterStart = CenterEnd;
	}

	//draw end line
	CenterEnd = SplineBuilder->GetPoint(1.0f);
	PDI->DrawLine(CenterEnd + LeftWidth, CenterEnd - LeftWidth, Color, SDPG_Foreground);


	/*
	//draw starting line
	CenterEnd = SplineBuilder->GetPointOnPath(Time);
	Direction = CenterEnd - CenterStart;
	Direction.Normalize();
	Left = FVector::CrossProduct(FVector::UpVector, Direction) * PathWidth;
	Right = -Left;
	PDI->DrawLine(CenterStart, CenterStart + Left, Color, SDPG_Foreground);
	PDI->DrawLine(CenterStart, CenterStart + Right, Color, SDPG_Foreground);

	while (Time <= 1.0f)
	{
		CenterEnd = SplineBuilder->GetPointOnPath(Time);
		Direction = CenterEnd - CenterStart;
		Direction.Normalize();
		Left = FVector::CrossProduct(FVector::UpVector, Direction) * PathWidth;
		Right = -Left;

		//draw inner lines
		float LaneSegment = 1.0f / LanesCount;
		float LaneTime = LaneSegment;
		while (LaneTime < 1.0f)
		{
			FVector LaneLastPoint = FMath::Lerp(CenterStart + Left, CenterStart + Right, LaneTime);
			FVector LaneCurrentPoint = FMath::Lerp(CenterEnd + Left, CenterEnd + Right, LaneTime);
			PDI->DrawLine(LaneLastPoint, LaneCurrentPoint, Color, SDPG_Foreground);
			LaneTime += LaneSegment;
		}

		//draw most outer lines
		PDI->DrawLine(CenterStart + Left, CenterEnd + Left, Color, SDPG_Foreground);
		PDI->DrawLine(CenterStart + Right, CenterEnd + Right, Color, SDPG_Foreground);

		CenterStart = CenterEnd;
		Time += Segmentation;

		//draw closing line
		if (Time >= 1.0f)
		{
			PDI->DrawLine(CenterStart, CenterStart + Left, Color, SDPG_Foreground);
			PDI->DrawLine(CenterStart, CenterStart + Right, Color, SDPG_Foreground);
		}
	}

	//draw starting arrows
	CenterStart = SplineBuilder->GetPointOnPath(0.01f);
	CenterEnd = SplineBuilder->GetPointOnPath(0.015f);
	Direction = CenterEnd - CenterStart;
	Direction.Normalize();
	Left = FVector::CrossProduct(FVector::UpVector, Direction) * PathWidth;
	Right = -Left;
	FVector LeftStart = CenterStart + Left;
	FVector LeftEnd = CenterEnd + Left;
	FVector RightStart = CenterStart + Right;
	FVector RightEnd = CenterEnd + Right;
	float Seg = 1.0f / (LanesCount * 2);
	Time = Seg;
	bool bSkip = false;
	while (Time < 1.0f)
	{
		if (!bSkip)
		{
			CenterStart = FMath::Lerp(LeftStart, RightStart, Time);
			CenterEnd = FMath::Lerp(LeftEnd, RightEnd, Time);
			Direction = CenterEnd - CenterStart;
			Direction.Normalize();
			Left = FVector::CrossProduct(FVector::UpVector, Direction) * (PathWidth / (LanesCount * 2));
			Right = -Left;
			PDI->DrawLine(CenterStart + Left, CenterEnd, FLinearColor::Yellow, SDPG_Foreground);
			PDI->DrawLine(CenterStart + Right, CenterEnd, FLinearColor::Yellow, SDPG_Foreground);
			bSkip = true;
		}
		else
		{
			bSkip = false;
		}
		Time += Seg;
	}
	*/

	//draw nodes handle
	if (bDrawHandles)
	{
		for (int i = 0; i < Nodes.Num(); i++)
		{
			PDI->DrawPoint(Nodes[i], Color, GrabHandleSize, SDPG_Foreground);
		}
	}

	//draw directional arrows
	for (int i = 1; i < Nodes.Num() - 1; i++)
	{
		FVector Dir = (Nodes[i + 1] - Nodes[i]).GetSafeNormal();
		float ArrowShaftLength = 300.f;
		float ArrowHeadLength = ArrowShaftLength / 2.f;
		float ArrowShaftWidth = ArrowShaftLength / 3.f;
		float ArrowHeadWidth = ArrowShaftLength / 4.f;

		LeftWidth = FVector::CrossProduct(Dir, FVector::UpVector) * ArrowShaftWidth;
		FVector ArrowHeadLeftWidth = FVector::CrossProduct(Dir, FVector::UpVector) * ArrowHeadWidth;
		/*
			  P6
			  /\
			 /  \
			/    \
		   /      \
		P4/_P2__P3_\P5
			| 	 |
			|	 |
			|	 |
			|____|
			P0  P1

		*/
		FVector P0 = Nodes[i] + LeftWidth;
		FVector P1 = Nodes[i] - LeftWidth;
		FVector P2 = Nodes[i] + LeftWidth + (Dir * ArrowShaftLength);
		FVector P3 = Nodes[i] - LeftWidth + (Dir * ArrowShaftLength);
		FVector P4 = P2 + ArrowHeadLeftWidth;
		FVector P5 = P3 - ArrowHeadLeftWidth;
		FVector P6 = Nodes[i] + Dir * (ArrowShaftLength + ArrowHeadLength);

		PDI->DrawLine(P0, P1, FLinearColor::Gray, SDPG_Foreground);
		PDI->DrawLine(P0, P2, FLinearColor::Gray, SDPG_Foreground);
		PDI->DrawLine(P1, P3, FLinearColor::Gray, SDPG_Foreground);
		//PDI->DrawLine(P2, P3, FLinearColor::Gray, SDPG_Foreground);

		PDI->DrawLine(P2, P4, FLinearColor::Gray, SDPG_Foreground);
		PDI->DrawLine(P3, P5, FLinearColor::Gray, SDPG_Foreground);

		PDI->DrawLine(P4, P6, FLinearColor::Gray, SDPG_Foreground);
		PDI->DrawLine(P5, P6, FLinearColor::Gray, SDPG_Foreground);


		// draw stop points

			/*
			   P6 ________ P7
				 /        \
			 P4 /          \ P5
			   |            |
			   |            |
			 P2 \          / P3
				 \________/
				 P0      P1

			*/

		FVector P0S = Nodes[i] + LeftWidth;
		FVector P1S = Nodes[i] - LeftWidth;

		PDI->DrawLine(P0S, P1S, FLinearColor::Red, SDPG_Foreground);

	}
}

FBoxSphereBounds UTrafficPath::CalcBounds(const FTransform& LocalToWorld) const
{
	if (!bDrawDebug)
	{
		// Do as little as possible if not rendering anything
		return Super::CalcBounds(LocalToWorld);
	}
	const FTransform& WorldTransform = GetComponentTransform();
	FBox BoundingBox(EForceInit::ForceInit);
	for (const FVector& InterpPoint : Nodes)
	{
		BoundingBox += (WorldTransform.InverseTransformPosition(InterpPoint));
	}
	return FBoxSphereBounds(BoundingBox.TransformBy(LocalToWorld));
}
#endif

#pragma endregion
/* End UTrafficPath*/