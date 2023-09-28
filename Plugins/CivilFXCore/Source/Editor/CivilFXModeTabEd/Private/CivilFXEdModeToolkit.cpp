// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CivilFXEdModeToolkit.h"
#include "CivilFXEdMode.h"
#include "CivilFXEdModeSubPanelSlates.h"

#include "EditorModeManager.h"

#include "SlateOptMacros.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "CivilFXToolbarEdStyle.h"

#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "EditorModeManager.h"
#include "Modules/ModuleManager.h"

#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "FCivilFXEdModeToolkit"

/*commands implementation*/
FName FCivilFXEdModeCommands::CivilFXEdModeContext = TEXT("CivilFXEdMode");

FCivilFXEdModeCommands::FCivilFXEdModeCommands() :
	TCommands<FCivilFXEdModeCommands>
	(
		FCivilFXEdModeCommands::CivilFXEdModeContext,
		NSLOCTEXT("Contexts", "CivilFXEdMode", "CivilFX Mode Editor"),
		NAME_None,
		FCivilFXToolbarEdStyle::GetStyleSetName()
	)
{
}

void FCivilFXEdModeCommands::RegisterCommands()
{
	UI_COMMAND(CameraMode, "Mode - Camera", "", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(TreeMode, "Mode - Tree", "", EUserInterfaceActionType::RadioButton, FInputChord());
}


/*********/

/*slate widget implementation*/
void SCivilFXEdModeWidget::Construct(const FArguments& InArgs, TSharedRef<FCivilFXEdModeToolkit> InParentToolkit)
{
	TSharedRef<FUICommandList> CommandList = InParentToolkit->GetToolkitCommands();

	//Modes
	FToolBarBuilder ModeSwitchButtons(CommandList, FMultiBoxCustomization::None);
	{
		ModeSwitchButtons.AddToolBarButton(FCivilFXEdModeCommands::Get().CameraMode, 
			NAME_None, 
			LOCTEXT("Mode.Camera", "Camera"),
			LOCTEXT("Mode.Camera.Tooltip","Contains tools to add new still/animated cameras"),
			FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXEdMode.CameraIcon")
		);

		ModeSwitchButtons.AddToolBarButton(FCivilFXEdModeCommands::Get().TreeMode, 
			NAME_None, 
			LOCTEXT("Mode.Tree", "Tree"), 
			LOCTEXT("Mode.Tree.Tooltip", "Contains tools to add new static mesh instances"),
			FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXEdMode.TreeIcon")
		);
	}
	
	/*
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);
	DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	//set default setting object (camera)
	DetailsPanel->SetObject(SettingsObjects[0], true);
	*/

	SAssignNew(WidgetSwitcher, SWidgetSwitcher)
	+SWidgetSwitcher::Slot()
	[
		SNew(SCameraSettingsModeWidget)
	]
	+SWidgetSwitcher::Slot()
	[
		SNew(SButton)
	];

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(4, 0, 4, 5)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.HAlign(HAlign_Center)
			[
				ModeSwitchButtons.MakeWidget()
			]
		]

		//seperator
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(3.0f)
		[
			SNew(SSeparator)
			.Orientation(Orient_Horizontal)
		]

		//actual content panel
		+ SVerticalBox::Slot()
		.Padding(0)
		[
			//DetailsPanel.ToSharedRef()
			WidgetSwitcher.ToSharedRef()
		]
	];
}


void SCivilFXEdModeWidget::NotifyToolChange(FName ToolName)
{
	if (ToolName == FCivilFXEdModeToolkit::CameraModeName)
	{
		//camera mode
		//DetailsPanel->SetObject(SettingsObjects[0], true);
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
	else if (ToolName == FCivilFXEdModeToolkit::TreeModeName)
	{
		//tree mode
		//DetailsPanel->SetObject(SettingsObjects[1], true);
		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}



/*Toolkit implementation*/

FName FCivilFXEdModeToolkit::CameraModeName = TEXT("CameraMode");
FName FCivilFXEdModeToolkit::TreeModeName = TEXT("TreeMode");

FCivilFXEdModeToolkit::FCivilFXEdModeToolkit() :
	CurrentModeName(CameraModeName)
{
}

void FCivilFXEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	//set default mode
	//CurrentModeName = CameraModeName;

	FCivilFXEdMode* CivilFXEdMode = (FCivilFXEdMode*)GetEditorMode();
	TSharedRef<FUICommandList> CommandList = CivilFXEdMode->GetUICommandList();
	check(CommandList->DoesSharedInstanceExist());
	CommandList->MapAction(FCivilFXEdModeCommands::Get().CameraMode, 
		FUIAction(FExecuteAction::CreateSP(this, &FCivilFXEdModeToolkit::OnChangeMode, CameraModeName),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FCivilFXEdModeToolkit::IsModeActive, CameraModeName)
		));
	CommandList->MapAction(FCivilFXEdModeCommands::Get().TreeMode,
		FUIAction(FExecuteAction::CreateSP(this, &FCivilFXEdModeToolkit::OnChangeMode, TreeModeName),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FCivilFXEdModeToolkit::IsModeActive, TreeModeName)
		));

	ToolkitWidget = SNew(SCivilFXEdModeWidget, SharedThis(this));

	FModeToolkit::Init(InitToolkitHost);
}

FName FCivilFXEdModeToolkit::GetToolkitFName() const
{
	return FName("CivilFXEdMode");
}

FText FCivilFXEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("CivilFXEdModeToolkit", "DisplayName", "CivilFXEdMode Tool");
}

FEdMode* FCivilFXEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FCivilFXEdMode::EM_CivilFXEdModeEdModeId);
}

void FCivilFXEdModeToolkit::OnChangeMode(FName NewModeName)
{
	CurrentModeName = NewModeName;
	ToolkitWidget->NotifyToolChange(NewModeName);
}

bool FCivilFXEdModeToolkit::IsModeActive(FName ModeName) const
{
	return CurrentModeName == ModeName;
}

#undef LOCTEXT_NAMESPACE