// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficPathControllerVisualizer.h"
#include "CivilFXCore/TrafficSim/TrafficPathController.h"

FTrafficPathControllerVisualizer::FTrafficPathControllerVisualizer() :
	FComponentVisualizer()
{
}

FTrafficPathControllerVisualizer::~FTrafficPathControllerVisualizer()
{
}

void FTrafficPathControllerVisualizer::OnRegister()
{

}

void FTrafficPathControllerVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const UTrafficPathController* PathController = Cast<UTrafficPathController>(Component);
	if (PathController)
	{
		UTrafficPathController::Draw(PDI, View, PathController);
	}
}

UTrafficPathController* FTrafficPathControllerVisualizer::GetEditedTargetingComponent() const
{
	return nullptr;
}

