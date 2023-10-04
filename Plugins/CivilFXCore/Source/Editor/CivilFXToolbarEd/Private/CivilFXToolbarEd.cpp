#include "CivilFXToolbarEd.h"

#include "CivilFXToolbarEdCommands.h"
//#include "Misc/MessageDialog.h"
//#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "CivilFXCore/CommonCore/PhaseElement.h"
#include "CivilFXToolbarEdStyle.h"

#include "LevelEditor.h" //FLevelEditorModule

#include "Framework/MultiBox/MultiBoxDefs.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "Modules/ModuleManager.h" // FModuleManager
#include "LevelEditorActions.h" //FLevelEditorCommands
#include "Features/IModularFeatures.h" //IModularFeatures
#include "Features/EditorFeatures.h" //EditorFeatures
#include "ISettingsModule.h" //ISettingsModule
#include "ToolMenus.h"//UToolMenu

#include "Engine/Engine.h"

static const FName CivilFXToolbarTabName("CivilFXToolbarEd");

#define LOCTEXT_NAMESPACE "FCivilFXToolbarEd"

void FCivilFXToolbarEd::StartupModule()
{
	//register icon
	FCivilFXToolbarEdStyle::Initialize();
	FCivilFXToolbarEdStyle::ReloadTextures();

	//register commands
	FCivilFXToolbarEdCommands::Register();

	//set up commands list
	Commands = MakeShareable(new FUICommandList);

	//mapping actions
	Commands->MapAction(
		FCivilFXToolbarEdCommands::Get().ProposedAction,
		FExecuteAction::CreateRaw(this, &FCivilFXToolbarEd::InvokePhase, EPhaseType::Proposed),
		FCanExecuteAction());

	//
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");


	/* Engine's code: No need anymore
	TSharedRef<FUICommandList> InCommands = Commands.ToSharedRef();
	UToolMenu* ToolbarBuilder = UToolMenus::Get()->RegisterMenu("LevelEditor.LevelEditorToolBar", NAME_None, EMultiBoxType::ToolBar);
	FToolMenuSection& Section = ToolbarBuilder->AddSection("CivilFX");
	Section.AddEntry(FToolMenuEntry::InitComboButton(
		"CivilFXComboButton",
		FUIAction(),
		FOnGetContent::CreateStatic(&FCivilFXToolbarEd::GenerateCivilFXMenu, InCommands),
		LOCTEXT("11", "CivilFX"),
		LOCTEXT("33", "CivilFX Features"),
		//FSlateIcon(FEditorStyle::GetStyleSetName(), "SourceControl.Actions.Connect")
		//FSlateIcon(FEditorToolbarButtonCFXStyle::GetStyleSetName(), "EditorToolbarButtonCFX.PluginAction")
		FSlateIcon()
	));
	*/
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Game", EExtensionHook::After, Commands, 
			FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& Builder) { 
				//lambda imp
			//add seperator
			Builder.AddSeparator();
			TSharedRef<FUICommandList> InCommands = Commands.ToSharedRef();
			Builder.BeginSection("CivilFX");
			Builder.AddComboButton(
				FUIAction(),
				FOnGetContent::CreateStatic(&FCivilFXToolbarEd::GenerateCivilFXMenu, InCommands),
				LOCTEXT("11", "CivilFX"),
				LOCTEXT("33", "CivilFX Features"),
				FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXToolbarEdStyle.CivilFXLogo")
			);
			Builder.EndSection();
		})
		
		);
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FCivilFXToolbarEd::ShutdownModule()
{
	FCivilFXToolbarEdStyle::Shutdown();
	FCivilFXToolbarEdCommands::Unregister();
}

TSharedRef<SWidget> FCivilFXToolbarEd::GenerateCivilFXMenu(TSharedRef<FUICommandList> InCommandList)
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<FExtender> MenuExtender = LevelEditorModule.AssembleExtenders(InCommandList, LevelEditorModule.GetAllLevelEditorToolbarBuildMenuExtenders());
	const bool bShouldCloseWindowAfterMenuSelection = true;
	FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, InCommandList, MenuExtender);

	//CivilFX toolbar button
	struct FCivilFXMenus
	{
		static void MakePhaseMenus(FMenuBuilder& InMenuBuilder)
		{
			InMenuBuilder.BeginSection("CivilFXPhaseMenu", LOCTEXT("Phase", "Phasing"));
			{
				InMenuBuilder.AddMenuEntry(FCivilFXToolbarEdCommands::Get().ProposedAction);
			}
			InMenuBuilder.EndSection();
		}
	};
	MenuBuilder.BeginSection("CivilFXPhaseMenu", LOCTEXT("Phase", "Phasing"));
	{
		MenuBuilder.AddSubMenu(
			LOCTEXT("1", "Phase"),
			LOCTEXT("3", "Select Current Phase"),
			FNewMenuDelegate::CreateStatic(&FCivilFXMenus::MakePhaseMenus)
		);
	}
	MenuBuilder.EndSection();
	return MenuBuilder.MakeWidget();
}



void FCivilFXToolbarEd::InvokePhase(EPhaseType InPhaseType)
{
#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%s: %s"), TEXT("Invoking Phase Editor"), *GETENUMSTRING("EPhaseType", InPhaseType)));
	for (TObjectIterator<AActor> ActorIt; ActorIt; ++ActorIt)
	{
		
		AActor* CurrentActor = *ActorIt;

		UPhaseElement* PhaseElement = (UPhaseElement*)CurrentActor->GetComponentByClass(UPhaseElement::StaticClass());
		if (PhaseElement)
		{
			GLog->Log(CurrentActor->GetName());
			//always single mode
			PhaseElement->DoPhase(InPhaseType, EPhaseMode::Single);
		}
	}
#undef GETENUMSTRING
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCivilFXToolbarEd, CivilFXToolbarEd)