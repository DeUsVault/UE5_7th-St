// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FComponentVisualizer;

class FTrafficSimEd : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void TRAFFICSIMED_API RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer);
	void TRAFFICSIMED_API RegisterClassCustomizations(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate);
	void TRAFFICSIMED_API RegisterStructCustomizations(FName StructName, FOnGetPropertyTypeCustomizationInstance DetailLayoutDelegate);
private:
	TArray<FName> RegisteredComponentClassNames;
	TArray<FName> RegisteredClassNames;
	TArray<FName> RegisteredStructNames;
};
