#include "CivilFXToolbarEdCommands.h"

#define LOCTEXT_NAMESPACE "FCivilFXToolbarEd"

void FCivilFXToolbarEdCommands::RegisterCommands()
{
	UI_COMMAND(ExistingAction, "Exising", "Switch to Existing Phase", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ProposedRecessedAction, "Proposed Recessed", "Switch to Proposed Recessed Phase", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ProposedNorthAction, "Proposed North", "Switch to Proposed North Phase", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ProposedSouthAction, "Proposed South", "Switch to Proposed South Phase", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE