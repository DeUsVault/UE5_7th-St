#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FCivilFXToolbarEdCommands : public TCommands<FCivilFXToolbarEdCommands>
{
public:
	FCivilFXToolbarEdCommands() :
		TCommands<FCivilFXToolbarEdCommands>(
			TEXT("CivilFXToolbarEd"),
			NSLOCTEXT("Contexts", "CivilFXToolbarEd", "CivilFXToolbarEd Commands"),
			NAME_None,
			FEditorStyle::GetStyleSetName())

		//: TCommands<FCivilFXToolbarEdCommands>(TEXT("EditorToolbarButtonCFX"), NSLOCTEXT("Contexts", "EditorToolbarButtonCFX", "EditorToolbarButtonCFX Plugin"), NAME_None, FEditorToolbarButtonCFXStyle::GetStyleSetName())
	{
	}
	
	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > ExistingAction;
	TSharedPtr<FUICommandInfo> ProposedRecessedAction;
	TSharedPtr< FUICommandInfo > ProposedNorthAction;
	TSharedPtr<FUICommandInfo> ProposedSouthAction;
};