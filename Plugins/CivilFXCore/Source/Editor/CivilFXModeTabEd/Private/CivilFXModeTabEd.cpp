
#include "CivilFXModeTabEd.h"
#include "CivilFXEdMode.h"
#include "CivilFXToolbarEdStyle.h"
#include "CivilFXToolbarEdCommands.h"
#include "CivilFXEdModeToolkit.h"

#include "CivilFXEdModeSubPanelSlates.h"

#define LOCTEXT_NAMESPACE "FCivilFXModeTabEd"

void FCivilFXModeTabEd::StartupModule()
{

	FCivilFXEdModeCommands::Register();
	FCameraSettingsModeCommands::Register();

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FCivilFXEdMode>(
		FCivilFXEdMode::EM_CivilFXEdModeEdModeId, 
		LOCTEXT("CivilFXEdModeName", "CivilFXEdMode Tools"), 
		FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXToolbarEdStyle.CivilFXLogo"), 
		//FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXEdMode.CameraIcon"),
		true);
}

void FCivilFXModeTabEd::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FCivilFXEdMode::EM_CivilFXEdModeEdModeId);


	FCivilFXEdModeCommands::Unregister();
	FCameraSettingsModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCivilFXModeTabEd, CivilFXModeTabEd)