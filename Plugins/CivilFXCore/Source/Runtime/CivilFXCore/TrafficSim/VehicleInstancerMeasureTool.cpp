// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleInstancerMeasureTool.h"
#include "Components/ActorComponent.h"
#include "PrimitiveSceneProxy.h" //FPrimitiveSceneProxy


UVehicleInstancerMeasureTool::UVehicleInstancerMeasureTool()
{

}

#if !UE_BUILD_SHIPPING

FPrimitiveSceneProxy* UVehicleInstancerMeasureTool::CreateSceneProxy()
{
	if (!bShowMeasureTool)
	{
		return Super::CreateSceneProxy();
	}

	class FSimpleLineSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FSimpleLineSceneProxy(const UVehicleInstancerMeasureTool* InComponent) :
			FPrimitiveSceneProxy(InComponent),
			bShowMeasureTool(InComponent->bShowMeasureTool),
			ToolComponent(InComponent)
		{}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_SplineSceneProxy_GetDynamicMeshElements);

			if (!IsSelected())
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
					UVehicleInstancerMeasureTool::Draw(PDI, View, ToolComponent, LocalToWorld, SDPG_World);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = bShowMeasureTool && IsSelected() && IsShown(View) && View->Family->EngineShowFlags.Splines;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
		uint32 GetAllocatedSize(void) const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

	private:
		bool bShowMeasureTool;
		const UVehicleInstancerMeasureTool* ToolComponent;
	};

	return new FSimpleLineSceneProxy(this);
}

void UVehicleInstancerMeasureTool::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View, const UVehicleInstancerMeasureTool* ToolComp, const FMatrix& LocalToWorld, uint8 DepthPriorityGroup)
{
	PDI->DrawLine(ToolComp->LineStartTransform.GetLocation(), ToolComp->LineEndTransform.GetLocation(), FLinearColor::Blue, DepthPriorityGroup, 10.0f);
}

FBoxSphereBounds UVehicleInstancerMeasureTool::CalcBounds(const FTransform& LocalToWorld) const
{
	if (!bShowMeasureTool)
	{
		// Do as little as possible if not rendering anything
		return Super::CalcBounds(LocalToWorld);
	}

	const FTransform& WorldTransform = GetComponentTransform();
	FBox BoundingBox(EForceInit::ForceInit);
	BoundingBox += WorldTransform.InverseTransformPosition(LineStartTransform.GetLocation());
	BoundingBox += WorldTransform.InverseTransformPosition(LineEndTransform.GetLocation());

	return FBoxSphereBounds(BoundingBox.TransformBy(LocalToWorld));
	/*
	GLog->Log(GetAttachParent()->CalcBounds(LocalToWorld).ToString());
	return GetAttachParent()->CalcBounds(LocalToWorld);
	
	FVector ArbitrarySize(500, 500, 500);
	return FBoxSphereBounds(ArbitrarySize, ArbitrarySize, 500);
	*/
}

#endif