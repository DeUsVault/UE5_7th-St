#pragma once

#include "ToolExampleEditor/IExampleModuleInterface.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"

class MenuTool : public IExampleModuleListenerInterface, public TSharedFromThis<MenuTool>
{
public:
	virtual ~MenuTool() {}

	virtual void OnStartupModule() override;
	virtual void OnShutdownModule() override;

	void MakeMenuEntry(FMenuBuilder &menuBuilder);
	void MakeSubMenu(FMenuBuilder &menuBuilder);


protected:
	TSharedPtr<FUICommandList> CommandList;

	void MapCommands();

	//************************
	// UI Command functions
	void MenuCommand1();
	void MenuCommand2();
	void MenuCommand3();
	void MenuCommand4();
	void MenuCommand5();
	void MenuCommand6();
	void MenuCommand7();

	FName TagToAdd;

	FReply AddTag();
	FText GetTagToAddText() const;
	void OnTagToAddTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

private:
	void InvokePhase(EPhaseType InPhaseType);
};