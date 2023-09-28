// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Misc/NotifyHook.h"

#include "Framework/Commands/Commands.h"

/*
*/
class FCivilFXEdModeToolkit;

/*Commands*/
class FCivilFXEdModeCommands : public TCommands<FCivilFXEdModeCommands>
{
public:
	FCivilFXEdModeCommands();

	virtual void RegisterCommands() override;

	static FName CivilFXEdModeContext;

	//mode switch
	TSharedPtr<FUICommandInfo> CameraMode;
	TSharedPtr<FUICommandInfo> TreeMode;

};

/*Slate widget*/
class SCivilFXEdModeWidget : public SCompoundWidget, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SCivilFXEdModeWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FCivilFXEdModeToolkit> InParentToolkit);

	void NotifyToolChange(FName ToolName);

protected:

	TSharedPtr<SWidgetSwitcher> WidgetSwitcher;
};


/********/
class FCivilFXEdModeToolkit : public FModeToolkit
{
public:
	FCivilFXEdModeToolkit();

	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }


	void OnChangeMode(FName NewModeName);
	bool IsModeActive(FName ModeName) const;

	static FName CameraModeName;
	static FName TreeModeName;


private:

	TSharedPtr<SCivilFXEdModeWidget> ToolkitWidget;

	FName CurrentModeName;


};
