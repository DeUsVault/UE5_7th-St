// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CivilFXEdModeSubPanelSlates.h"

#include "CivilFXToolbarEdStyle.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Application/SlateApplication.h"

#include "EditorStyleSet.h"

#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "CameraSettingsMode"

FCameraSettingsModeCommands::FCameraSettingsModeCommands() :
	TCommands<FCameraSettingsModeCommands>
	(
		TEXT("CameraSettingsCommands"),
		NSLOCTEXT("Contexts", "CivilFXEdMode", "CivilFX Mode Editor"),
		NAME_None,
		FCivilFXToolbarEdStyle::GetStyleSetName()
		)
{

}

void FCameraSettingsModeCommands::RegisterCommands()
{
	UI_COMMAND(StillCameraMode, "StillCameraMode", "Still Camera", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(AnimatedCameraMode, "AnimatedCameraMode", "Animated Camera", EUserInterfaceActionType::ToggleButton, FInputChord());
}

void SStillCameraModeWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
	];
}


void SAnimatedCameraModeWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SNew(SOverlay)
		]
		+SVerticalBox::Slot()
		[
			SNew(SBorder)
		]
	];
}


void SCameraSettingsModeWidget::Construct(const FArguments& InArgs)
{
	UICommandList = MakeShareable(new FUICommandList);

	UICommandList->MapAction(
		FCameraSettingsModeCommands::Get().StillCameraMode,
		FExecuteAction::CreateRaw(this, &SCameraSettingsModeWidget::OnNewCameraModeSelected, ECameraSettingsMode::StillCamera),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([=]
		{
			return CurrentSelectedMode == ECameraSettingsMode::StillCamera;
		}
	));
	UICommandList->MapAction(
		FCameraSettingsModeCommands::Get().AnimatedCameraMode,
		FExecuteAction::CreateRaw(this, &SCameraSettingsModeWidget::OnNewCameraModeSelected, ECameraSettingsMode::AnimatedCamera),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([=]
		{
			return CurrentSelectedMode == ECameraSettingsMode::AnimatedCamera;
		}
	));


	CameraModeSwitcher = BuildCameraModeSwitcher();

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(1.f, 5.f, 0.f, 5.f)
		[
			BuildToolBar()
		]

		+ SHorizontalBox::Slot()
		.Padding(0.f, 2.f, 2.f, 0.f)
		[
			CameraModeSwitcher.ToSharedRef()
		]
	];

}

TSharedRef<SWidget> SCameraSettingsModeWidget::BuildToolBar()
{
	FToolBarBuilder Toolbar(UICommandList, FMultiBoxCustomization::None, nullptr, Orient_Vertical);
	Toolbar.SetLabelVisibility(EVisibility::Collapsed);
	Toolbar.SetStyle(&FEditorStyle::Get(), "FoliageEditToolbar");
	{
		Toolbar.AddToolBarButton(FCameraSettingsModeCommands::Get().StillCameraMode,
			NAME_None,
			LOCTEXT("Mode.StillCamera", "Still Camera"),
			LOCTEXT("Mode.StillCamera.Tooltip", "Add/Remove Still Camera in Editor"),
			FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXEdMode.StillCameraIcon.Small")
		);

		Toolbar.AddToolBarButton(FCameraSettingsModeCommands::Get().AnimatedCameraMode,
			NAME_None,
			LOCTEXT("Mode.AnimatedCamera", "Animated Camera"),
			LOCTEXT("Mode.AnimatedCamera.Tooltip", "Add/Remove Animated Camera in Editor"),
			FSlateIcon(FCivilFXToolbarEdStyle::GetStyleSetName(), "CivilFXEdMode.AnimatedCameraIcon.Small")
		);
	}

	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBorder)
				.HAlign(HAlign_Center)
				.Padding(0)
				.BorderImage(FEditorStyle::GetBrush("NoBorder"))
				.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
				[
					Toolbar.MakeWidget()
				]
			]
		];
}

TSharedPtr<SWidgetSwitcher> SCameraSettingsModeWidget::BuildCameraModeSwitcher()
{
	return
		SNew(SWidgetSwitcher)
		+ SWidgetSwitcher::Slot()
		[
			SNew(SStillCameraModeWidget)
		]
		+ SWidgetSwitcher::Slot()
		[
			SNew(SAnimatedCameraModeWidget)
		]
	;
}

void SCameraSettingsModeWidget::OnNewCameraModeSelected(ECameraSettingsMode NewMode)
{
	CurrentSelectedMode = NewMode;
	CameraModeSwitcher->SetActiveWidgetIndex((int32)NewMode);
}

#undef LOCTEXT_NAMESPACE
