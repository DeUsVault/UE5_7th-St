// Fill out your copyright notice in the Description page of Project Settings.

#include "ToolExampleEditor.h"
#include "MenuTool/MenuTool.h"


IMPLEMENT_GAME_MODULE(FToolExampleEditor, ToolExampleEditor)

TSharedRef<FWorkspaceItem> FToolExampleEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FToolExampleEditor::AddModuleListeners()
{
	// add tools
	ModuleListeners.Add(MakeShareable(new MenuTool));
}

void FToolExampleEditor::StartupModule()
{
	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
		MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FToolExampleEditor::MakePulldownMenu));
		LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}
	
	IExampleModuleInterface::StartupModule();
}

void FToolExampleEditor::ShutdownModule()
{	
	IExampleModuleInterface::ShutdownModule();
}


void FToolExampleEditor::AddMenuExtension(const FMenuExtensionDelegate &extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList> &CommandList, EExtensionHook::Position position)
{
	MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

void FToolExampleEditor::MakePulldownMenu(FMenuBarBuilder &menuBuilder)
{
	menuBuilder.AddPullDownMenu(
		FText::FromString("Phase"),
		FText::FromString("Open the Phase menu"),
		FNewMenuDelegate::CreateRaw(this, &FToolExampleEditor::FillPulldownMenu),
		"Example",
		FName(TEXT("ExampleMenu"))
	);
}

void FToolExampleEditor::FillPulldownMenu(FMenuBuilder &menuBuilder)
{
	menuBuilder.BeginSection("ExampleSection", FText::FromString(""));
	menuBuilder.AddMenuSeparator(FName("Section_1"));
	menuBuilder.EndSection();

	//menuBuilder.BeginSection("ExampleSection", FText::FromString("Section 2"));
	//menuBuilder.AddMenuSeparator(FName("Section_2"));
	//menuBuilder.EndSection();
}
