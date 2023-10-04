#include "CivilFXToolbarEdCommands.h"

#define LOCTEXT_NAMESPACE "FCivilFXToolbarEd"

void FCivilFXToolbarEdCommands::RegisterCommands()
{
	UI_COMMAND(ProposedAction, "Proposed", "Switch to Proposed Phase", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE