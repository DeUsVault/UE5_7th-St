// Fill out your copyright notice in the Description page of Project Settings.
#include "TrafficPathVisualizer.h"
#include "CivilFXCore/TrafficSim/TrafficPath.h"
#include "EditorViewportCommands.h" //TCommands
#include "Framework/Commands/UICommandList.h" //FUICommands
#include "ActorEditorUtils.h" //FActorEditorUtils
#include "EditorViewportClient.h" //FViewportClick
#include "ScopedTransaction.h" //Transaction
#include "EditorStyleSet.h" //FEditorStyle

IMPLEMENT_HIT_PROXY(HNodeProxy, HComponentVisProxy);

#define LOCTEXT_NAMESPACE "TrafficPathVisualizer"


/** Define commands for the spline component visualizer */
class FSplineComponentVisualizerCommands : public TCommands<FSplineComponentVisualizerCommands>
{
public:
	FSplineComponentVisualizerCommands() : TCommands <FSplineComponentVisualizerCommands>
		(
			"TrafficPathVisualizer",	// Context name for fast lookup
			LOCTEXT("TrafficPathVisualizer", "Traffic Path Visualizer"),	// Localized context name for displaying
			NAME_None,	// Parent
			FEditorStyle::GetStyleSetName()
			)
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(DeleteKey, "Delete Node", "Delete the currently selected node.", EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
		UI_COMMAND(DeleteKeyX, "Delete Node", "Delete the currently selected node.", EUserInterfaceActionType::Button, FInputChord(EKeys::X));
		UI_COMMAND(InsertKey, "Insert Node", "Insert a new node.", EUserInterfaceActionType::Button, FInputChord(EKeys::C));
		UI_COMMAND(ProjectNodesKey, "Project Nodes", "Project All Nodes Onto Mesh", EUserInterfaceActionType::Button, FInputChord(EKeys::SpaceBar));
	}

public:
	/** Delete key */
	TSharedPtr<FUICommandInfo> DeleteKey; //mapped to key "Delete"
	TSharedPtr<FUICommandInfo> DeleteKeyX; //mapped to key "X"

	/** Insert key */
	TSharedPtr<FUICommandInfo> InsertKey; //mapped to key "C"

	/** Spacebar */
	TSharedPtr<FUICommandInfo> ProjectNodesKey; //mapped to key "SpaceBar"
};


FTrafficPathVisualizer::FTrafficPathVisualizer() :
	FComponentVisualizer(),
	SelectedNodeIndex(INDEX_NONE),
	UpdateNodeMode(EUpdateNodesMode::Single)
{
	FSplineComponentVisualizerCommands::Register();
	SplineComponentVisualizerActions = MakeShareable(new FUICommandList);
	NodesProperty = FindField<FProperty>(UTrafficPath::StaticClass(), TEXT("Nodes"));
}

FTrafficPathVisualizer::~FTrafficPathVisualizer()
{
	FSplineComponentVisualizerCommands::Unregister();
}

void FTrafficPathVisualizer::OnRegister()
{
	const auto& Commands = FSplineComponentVisualizerCommands::Get();
	SplineComponentVisualizerActions->MapAction(
		Commands.DeleteKey,
		FExecuteAction::CreateSP(this, &FTrafficPathVisualizer::OnDeleteNode),
		FCanExecuteAction::CreateSP(this, &FTrafficPathVisualizer::CanDeleteNode));

	SplineComponentVisualizerActions->MapAction(
		Commands.DeleteKeyX,
		FExecuteAction::CreateSP(this, &FTrafficPathVisualizer::OnDeleteNode),
		FCanExecuteAction::CreateSP(this, &FTrafficPathVisualizer::CanDeleteNode));

	SplineComponentVisualizerActions->MapAction(
		Commands.InsertKey,
		FExecuteAction::CreateSP(this, &FTrafficPathVisualizer::OnInsertNode),
		FCanExecuteAction::CreateSP(this, &FTrafficPathVisualizer::CanInsertNode));

	SplineComponentVisualizerActions->MapAction(
		Commands.ProjectNodesKey,
		FExecuteAction::CreateSP(this, &FTrafficPathVisualizer::OnProjectNodes),
		FCanExecuteAction::CreateSP(this, &FTrafficPathVisualizer::CanProjectNodes));
}

void FTrafficPathVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (const UTrafficPath* TrafficPathComp = Cast<const UTrafficPath>(Component))
	{
		UTrafficPath::Draw(PDI, View, TrafficPathComp, false);

		const float GrabHandleSize = 10.0f;
		const TArray<FVector>& Nodes = TrafficPathComp->GetNodes();
		const FLinearColor& Color = TrafficPathComp->GetSplineColor();
		for (int i=0; i<Nodes.Num(); ++i)
		{
			PDI->SetHitProxy(new HNodeProxy(Component, i));
			PDI->DrawPoint(Nodes[i], Color, GrabHandleSize, SDPG_Foreground);
			PDI->SetHitProxy(NULL);
		}
	}

}

bool FTrafficPathVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	bool bEditing = false;
	if (VisProxy && VisProxy->Component.IsValid())
	{
		UTrafficPath* TrafficPathComp = (UTrafficPath*)(VisProxy->Component.Get());
		AActor* OldOwningActor = SplinePropertyPath.GetParentOwningActor();
		SplinePropertyPath = FComponentPropertyPath(TrafficPathComp);
		AActor* NewOwningActor = SplinePropertyPath.GetParentOwningActor();
		if (SplinePropertyPath.IsValid())
		{
			if (OldOwningActor != NewOwningActor)
			{
				SelectedNodeIndex = INDEX_NONE;
			}
			if (VisProxy->IsA(HNodeProxy::StaticGetType()))
			{
				//control point clicked
				HNodeProxy* KeyProxy = (HNodeProxy*)VisProxy;
				if (Click.GetKey() != EKeys::RightMouseButton)
				{
					SelectedNodeIndex = KeyProxy->NodeIndex;
					bEditing = true;
				}
			}
		}
		else
		{
			SplinePropertyPath.Reset();
		}
	}
	else
	{
		SelectedNodeIndex = INDEX_NONE;
	}

	return bEditing;
}

void FTrafficPathVisualizer::EndEditing()
{
	SplinePropertyPath.Reset();
	SelectedNodeIndex = INDEX_NONE;
}

bool FTrafficPathVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	if (TrafficPathComp != nullptr && SelectedNodeIndex != INDEX_NONE)
	{
		OutLocation = TrafficPathComp->GetNodes()[SelectedNodeIndex];
		return true;
	}
	return false;
}

bool FTrafficPathVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
{
	bool bHandled = false;
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	if (TrafficPathComp != nullptr)
	{

		TrafficPathComp->Modify();
		if (UpdateNodeMode == EUpdateNodesMode::Single && SelectedNodeIndex != INDEX_NONE)
		{
			FVector NewNode = TrafficPathComp->GetNodes()[SelectedNodeIndex] + DeltaTranslate;
			TrafficPathComp->GetNodes()[SelectedNodeIndex] = GetProjectedNode(NewNode);
		}
		else if (UpdateNodeMode == EUpdateNodesMode::Patch)
		{
			for (FVector& Node : TrafficPathComp->GetNodes())
			{
				Node += DeltaTranslate;
			}
		}
		bHandled = true;
		NotifyPropertyModified(TrafficPathComp, NodesProperty);
		GEditor->RedrawLevelEditingViewports(true);
	}
	return bHandled;
}

bool FTrafficPathVisualizer::HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool bHandled = false;
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();

	if (TrafficPathComp != nullptr)
	{
		//double click to add a node
		if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_DoubleClick)
		{
			OnInsertNode();
			bHandled = true;
		}

		if (Key == EKeys::LeftAlt)
		{
			if (Event == EInputEvent::IE_Pressed)
			{
				UpdateNodeMode = EUpdateNodesMode::Patch;
			}
			else if (Event == EInputEvent::IE_Released)
			{
				UpdateNodeMode = EUpdateNodesMode::Single;
			}
			bHandled = true;
		}
		
	}

	if (Event == IE_Pressed)
	{
		bHandled = SplineComponentVisualizerActions->ProcessCommandBindings(Key, FSlateApplication::Get().GetModifierKeys(), false);
	}
	return bHandled;
}

TSharedPtr<SWidget> FTrafficPathVisualizer::GenerateContextMenu() const
{
	return TSharedPtr<SWidget>();
}

bool FTrafficPathVisualizer::IsVisualizingArchetype() const
{
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	return (TrafficPathComp && TrafficPathComp->GetOwner() && FActorEditorUtils::IsAPreviewOrInactiveActor(TrafficPathComp->GetOwner()));
}

UTrafficPath* FTrafficPathVisualizer::GetEditedTargetingComponent() const
{
	return Cast<UTrafficPath>(SplinePropertyPath.GetComponent());
}


void FTrafficPathVisualizer::OnDeleteNode()
{
	const FScopedTransaction Transaction(LOCTEXT("DeleteNode", "Delete Path Node"));

	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	if (TrafficPathComp != nullptr)
	{
		TArray<FVector>& Nodes = TrafficPathComp->GetNodes();

		if (Nodes.Num() > UTrafficPath::MIN_NODES_COUNT && SelectedNodeIndex != INDEX_NONE)
		{
			TrafficPathComp->Modify();
			if (AActor* Owner = TrafficPathComp->GetOwner())
			{
				Owner->Modify();
			}
			//do remove
			Nodes.RemoveAt(SelectedNodeIndex);

			SelectedNodeIndex = SelectedNodeIndex == 0 ? 0 : SelectedNodeIndex - 1;

			NotifyPropertyModified(TrafficPathComp, NodesProperty);

			//force rebuild spline
			TrafficPathComp->GetSplineBuilder(true);
			GEditor->RedrawLevelEditingViewports(true);
		}
	}
}

bool FTrafficPathVisualizer::CanDeleteNode() const
{
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	return (TrafficPathComp != nullptr &&
		SelectedNodeIndex != INDEX_NONE);
}

void FTrafficPathVisualizer::OnInsertNode()
{
	const FScopedTransaction Transaction(LOCTEXT("InsertNode", "Insert New Node"));
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	if (TrafficPathComp != nullptr)
	{
		TArray<FVector>& Nodes = TrafficPathComp->GetNodes();

		if (SelectedNodeIndex != INDEX_NONE)
		{
			TrafficPathComp->Modify();
			if (AActor* Owner = TrafficPathComp->GetOwner())
			{
				Owner->Modify();
			}

			//do insert
			FVector Direction;
			FVector NewNode;
			float Distance;
			int32 NodesCount = Nodes.Num();
			if (SelectedNodeIndex == NodesCount - 1)
			{
				Direction = (Nodes[NodesCount - 1] - Nodes[NodesCount - 2]);
				Distance = Direction.Size();
				Direction.Normalize();
				NewNode = Nodes[NodesCount - 1] + (Direction * Distance);
			}
			else if (SelectedNodeIndex == 0)
			{
				Direction = (Nodes[1] - Nodes[0]);
				Distance = Direction.Size();
				Direction.Normalize();
				NewNode = Nodes[0] + (-Direction * Distance);
			}
			else
			{
				Direction = (Nodes[SelectedNodeIndex + 1] - Nodes[SelectedNodeIndex]);
				Direction.Normalize();
				Distance = FVector::Dist(Nodes[SelectedNodeIndex], Nodes[SelectedNodeIndex + 1]) / 2;
				NewNode = Nodes[SelectedNodeIndex] + (Direction * Distance);
			}
			SelectedNodeIndex = SelectedNodeIndex == 0 ? SelectedNodeIndex : SelectedNodeIndex + 1;
			NewNode = GetProjectedNode(NewNode);
			Nodes.Insert(NewNode, SelectedNodeIndex);

			NotifyPropertyModified(TrafficPathComp, NodesProperty);
			//force rebuild spline
			TrafficPathComp->GetSplineBuilder(true);
			GEditor->RedrawLevelEditingViewports(true);
		}
	}
}

bool FTrafficPathVisualizer::CanInsertNode() const
{
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	return (TrafficPathComp != nullptr &&
		SelectedNodeIndex != INDEX_NONE);
}

void FTrafficPathVisualizer::OnProjectNodes()
{
	const FScopedTransaction Transaction(LOCTEXT("ProjectNodes", "Project All Nodes Onto Mesh"));
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	if (TrafficPathComp != nullptr)
	{
		TArray<FVector>& Nodes = TrafficPathComp->GetNodes();

		TrafficPathComp->Modify();
		if (AActor* Owner = TrafficPathComp->GetOwner())
		{
			Owner->Modify();
		}

		//do project
		for (FVector& Node : Nodes)
		{
			Node = GetProjectedNode(Node);
		}

		NotifyPropertyModified(TrafficPathComp, NodesProperty);
		//force rebuild spline
		TrafficPathComp->GetSplineBuilder(true);
		GEditor->RedrawLevelEditingViewports(true);
	}
}

bool FTrafficPathVisualizer::CanProjectNodes() const
{
	UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	return (TrafficPathComp != nullptr);
}

FVector FTrafficPathVisualizer::GetProjectedNode(const FVector& CurrentNode, float TraceDistance/*=10.f*/) const
{
	const UTrafficPath* TrafficPathComp = GetEditedTargetingComponent();
	if (TrafficPathComp == nullptr)
	{
		return CurrentNode;
	}

	FHitResult Hit(1.0f);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ProjectingNode), true);
	if (TrafficPathComp->GetWorld()->LineTraceSingleByChannel(Hit, CurrentNode + FVector::UpVector * TraceDistance, CurrentNode + FVector::DownVector * WORLD_MAX, ECC_WorldStatic, Params))
	{
		return Hit.Location;
	}
	else
	{
		//if we don't hit anything try one more time
		//by moving the node up (TraceDistance * 2.0) unit and cast down again
		FVector NewNode = CurrentNode + FVector::UpVector * TraceDistance * 2.0f;
		if (TrafficPathComp->GetWorld()->LineTraceSingleByChannel(Hit, NewNode, NewNode + FVector::DownVector * WORLD_MAX, ECC_WorldStatic, Params))
		{
			return Hit.Location;
		}
	}
	return CurrentNode;
}

#undef LOCTEXT_NAMESPACE