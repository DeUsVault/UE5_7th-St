#pragma once

#include "CoreMinimal.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBarBuilder;
class FUICommandList;

class FCivilFXToolbarEd : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/**/

protected:
	/**
	* Generates menu content for the quick settings combo button drop down menu
	*
	* @return	Menu content widget
	*/
	static TSharedRef<SWidget> GenerateCivilFXMenu(TSharedRef<FUICommandList> InCommands);

private:
	TSharedPtr<FUICommandList> Commands;
	void InvokePhase(EPhaseType InPhaseType);
};