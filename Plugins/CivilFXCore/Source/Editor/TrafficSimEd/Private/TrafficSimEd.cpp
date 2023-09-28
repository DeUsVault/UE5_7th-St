// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TrafficSimEd.h"

#include "ComponentVisualizer.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "CivilFXCore/TrafficSim/TrafficPath.h"
#include "TrafficPathVisualizer.h"
#include "TrafficPathDetails.h"
#include "CivilFXCore/TrafficSim/TrafficPathController.h"
#include "TrafficPathControllerVisualizer.h"
#include "TrafficPathControllerDetails.h"

#define LOCTEXT_NAMESPACE "FTrafficSim"

void FTrafficSimEd::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	RegisterComponentVisualizer(UTrafficPath::StaticClass()->GetFName(), MakeShareable(new FTrafficPathVisualizer));
	RegisterComponentVisualizer(UTrafficPathController::StaticClass()->GetFName(), MakeShareable(new FTrafficPathControllerVisualizer));

	RegisterClassCustomizations("TrafficPath", FOnGetDetailCustomizationInstance::CreateStatic(&FTrafficPathDetails::MakeInstance));
	//RegisterObjectCustomizations("TrafficPathController", FOnGetDetailCustomizationInstance::CreateStatic(&FTrafficPathControllerDetails::MakeInstance));

	RegisterStructCustomizations("Ramp", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRampCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FTrafficSimEd::ShutdownModule()
{
	if (GUnrealEd != NULL)
	{
		for (const FName& ClassName : RegisteredComponentClassNames)
		{
			GUnrealEd->UnregisterComponentVisualizer(ClassName);
		}
	}


	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		// Unregister all classes customized by name
		for (auto It = RegisteredClassNames.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomClassLayout(*It);
			}
		}

		// Unregister all structs customized by name
		for (auto It = RegisteredStructNames.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomPropertyTypeLayout(*It);
			}
		}

		PropertyModule.NotifyCustomizationModuleChanged();
	}

}

void FTrafficSimEd::RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer)
{
	if (GUnrealEd != NULL)
	{
		GUnrealEd->RegisterComponentVisualizer(ComponentClassName, Visualizer);
	}

	RegisteredComponentClassNames.Add(ComponentClassName);

	if (Visualizer.IsValid())
	{
		Visualizer->OnRegister();
	}
}

void FTrafficSimEd::RegisterClassCustomizations(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate)
{
	check(ClassName != NAME_None);

	RegisteredClassNames.Add(ClassName);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomClassLayout(ClassName, DetailLayoutDelegate);
}

void TRAFFICSIMED_API FTrafficSimEd::RegisterStructCustomizations(FName StructName, FOnGetPropertyTypeCustomizationInstance DetailLayoutDelegate)
{
	check(StructName != NAME_None);

	RegisteredStructNames.Add(StructName);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomPropertyTypeLayout(StructName, DetailLayoutDelegate);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTrafficSimEd, TrafficSimEd)