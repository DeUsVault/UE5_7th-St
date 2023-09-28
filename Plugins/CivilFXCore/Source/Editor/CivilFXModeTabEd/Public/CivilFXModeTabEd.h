
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCivilFXModeTabEd : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};