// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"
/**
 * 
 */
class TRAFFICSIMED_API FTrafficPathControllerVisualizer : public FComponentVisualizer
{
public:
	FTrafficPathControllerVisualizer();
	virtual ~FTrafficPathControllerVisualizer();

	//~ Begin FComponentVisualizer Interface
	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	//~ End FComponentVisualizer Interface

	/** Get the target component we are currently editing */
	class UTrafficPathController* GetEditedTargetingComponent() const;
protected:
	FComponentPropertyPath ComponentPropPath;
};
