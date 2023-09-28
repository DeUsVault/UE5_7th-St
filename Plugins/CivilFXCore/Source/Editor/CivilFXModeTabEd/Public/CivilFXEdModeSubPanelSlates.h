// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"


/** Camera Mode Widget stuffs */
class FCameraSettingsModeCommands : public TCommands<FCameraSettingsModeCommands>
{
public:
	FCameraSettingsModeCommands();

	virtual void RegisterCommands() override;

	//mode switch
	TSharedPtr<FUICommandInfo> StillCameraMode;
	TSharedPtr<FUICommandInfo> AnimatedCameraMode;

};

enum class ECameraSettingsMode : uint8
{
	StillCamera = 0,
	AnimatedCamera
};


class SStillCameraModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStillCameraModeWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};


class SAnimatedCameraModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAnimatedCameraModeWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};

class SCameraSettingsModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCameraSettingsModeWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
private:
	TSharedPtr<FUICommandList> UICommandList;
	TSharedRef<SWidget> BuildToolBar();
	TSharedPtr<SWidgetSwitcher> BuildCameraModeSwitcher();
	TSharedPtr<SWidgetSwitcher> CameraModeSwitcher;

	void OnNewCameraModeSelected(ECameraSettingsMode NewMode);

	ECameraSettingsMode CurrentSelectedMode;

};

/** ~End Camera Mode Widget stuffs */