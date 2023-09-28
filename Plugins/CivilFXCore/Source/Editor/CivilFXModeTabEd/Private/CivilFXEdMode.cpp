// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

//#include "TestModesTabEdMode.h"
//#include "TestModesTabEdModeToolkit.h"

#include "CivilFXEdMode.h"

#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "CivilFXEdModeToolkit.h"

const FEditorModeID FCivilFXEdMode::EM_CivilFXEdModeEdModeId = TEXT("EM_CivilFXEdMode");

FCivilFXEdMode::FCivilFXEdMode()
{

}

FCivilFXEdMode::~FCivilFXEdMode()
{

}

void FCivilFXEdMode::Enter()
{
	FEdMode::Enter();

	
	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FCivilFXEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
	
}

void FCivilFXEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FCivilFXEdMode::UsesToolkits() const
{
	return true;
}

TSharedRef<FUICommandList> FCivilFXEdMode::GetUICommandList() const
{
	check(Toolkit.IsValid());
	return Toolkit->GetToolkitCommands();
}




