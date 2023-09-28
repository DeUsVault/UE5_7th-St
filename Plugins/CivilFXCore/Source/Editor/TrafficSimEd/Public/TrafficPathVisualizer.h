// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "HitProxies.h"
#include "ComponentVisualizer.h"
#include "Components/SplineComponent.h"

class UTrafficPath;

struct HNodeProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HNodeProxy(const UActorComponent* InComponent, int32 InSelectedNodeIndex) :
		HComponentVisProxy(InComponent, HPP_Wireframe),
		NodeIndex(InSelectedNodeIndex)
	{
	}
	int32 NodeIndex;
};


class TRAFFICSIMED_API FTrafficPathVisualizer : public FComponentVisualizer
{
	enum class EUpdateNodesMode
	{
		Single,
		Patch
	};


public:
	FTrafficPathVisualizer();
	virtual ~FTrafficPathVisualizer();

	//~ Begin FComponentVisualizer Interface
	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	virtual void EndEditing() override;
	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	//virtual bool GetCustomInputCoordinateSystem(const FEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const override;
	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;
	virtual bool HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual TSharedPtr<SWidget> GenerateContextMenu() const override;
	virtual bool IsVisualizingArchetype() const override;
	//~ End FComponentVisualizer Interface

	/** Get the target component we are currently editing */
	UTrafficPath* GetEditedTargetingComponent() const;

protected:
	FComponentPropertyPath SplinePropertyPath;
	FProperty* NodesProperty;

	void OnDeleteNode();
	bool CanDeleteNode() const;

	void OnInsertNode();
	bool CanInsertNode() const;

	void OnProjectNodes();
	bool CanProjectNodes() const;

private:

	EUpdateNodesMode UpdateNodeMode;

	/**Index of target in selected component*/
	int32 SelectedNodeIndex;

	/**Output log commands*/
	TSharedPtr<FUICommandList> SplineComponentVisualizerActions;

	FVector GetProjectedNode(const FVector& CurrentNode, float TraceDistance=100.f) const;
};
