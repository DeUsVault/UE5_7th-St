#include "MenuTool.h"
#include "ToolExampleEditor/ToolExampleEditor.h"
#include "ScopedTransaction.h"
#include "CivilFXCore/CommonCore/PhaseElement.h"
#include "EditorStyleSet.h"


#define LOCTEXT_NAMESPACE "MenuTool"

class MenuToolCommands : public TCommands<MenuToolCommands>
{
public:

	MenuToolCommands()
		: TCommands<MenuToolCommands>(
		TEXT("MenuTool"), // Context name for fast lookup
		FText::FromString("Example Menu tool"), // Context name for displaying
		NAME_None,	 // No parent context
		FEditorStyle::GetStyleSetName() // Icon Style Set
		)
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(MenuCommand1, "Existing", "Existing phase", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(MenuCommand2, "BRT", "BRT phase", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(MenuCommand3, "BRT No Ped", "BRT no ped phase", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(MenuCommand4, "LRT", "LRT phase", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(MenuCommand5, "LRT No Ped", "LRT no ped phase", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(MenuCommand6, "DMU", "DMU phase", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(MenuCommand7, "DMU No Ped", "DMU no ped phase", EUserInterfaceActionType::Button, FInputGesture());
	}

public:
	TSharedPtr<FUICommandInfo> MenuCommand1;
	TSharedPtr<FUICommandInfo> MenuCommand2;
	TSharedPtr<FUICommandInfo> MenuCommand3;
	TSharedPtr<FUICommandInfo> MenuCommand4;
	TSharedPtr<FUICommandInfo> MenuCommand5;
	TSharedPtr<FUICommandInfo> MenuCommand6;
	TSharedPtr<FUICommandInfo> MenuCommand7;
};

void MenuTool::MapCommands()
{
	const auto& Commands = MenuToolCommands::Get();

	CommandList->MapAction(
		Commands.MenuCommand1,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand1),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.MenuCommand2,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand2),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.MenuCommand3,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand3),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.MenuCommand4,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand4),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.MenuCommand5,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand5),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.MenuCommand6,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand6),
		FCanExecuteAction());

	CommandList->MapAction(
		Commands.MenuCommand7,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand7),
		FCanExecuteAction());
}


void MenuTool::OnStartupModule()
{
	CommandList = MakeShareable(new FUICommandList);
	MenuToolCommands::Register();
	MapCommands();
	FToolExampleEditor::Get().AddMenuExtension(
		FMenuExtensionDelegate::CreateRaw(this, &MenuTool::MakeMenuEntry),
		FName("Section_1"),
		CommandList);
}

void MenuTool::OnShutdownModule()
{
	MenuToolCommands::Unregister();
}


void MenuTool::MakeMenuEntry(FMenuBuilder &menuBuilder)
{
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand1);
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand2);
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand3);
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand4);
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand5);
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand6);
	menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand7);

	//menuBuilder.AddSubMenu(
	//	FText::FromString("Sub Menu"),
	//	FText::FromString("This is example sub menu"),
	//	FNewMenuDelegate::CreateSP(this, &MenuTool::MakeSubMenu)
	//);

	//// add tag
	//TSharedRef<SWidget> AddTagWidget =
	//	SNew(SHorizontalBox)
	//	+ SHorizontalBox::Slot()
	//	.AutoWidth()
	//	.VAlign(VAlign_Center)
	//	[
	//		SNew(SEditableTextBox)
	//		.MinDesiredWidth(50)
	//		.Text(this, &MenuTool::GetTagToAddText)
	//		.OnTextCommitted(this, &MenuTool::OnTagToAddTextCommited)
	//	]
	//	+ SHorizontalBox::Slot()
	//	.AutoWidth()
	//	.Padding(5, 0, 0, 0)
	//	.VAlign(VAlign_Center)
	//	[
	//		SNew(SButton)
	//		.Text(FText::FromString("Add Tag"))
	//		.OnClicked(this, &MenuTool::AddTag)
	//	];

	//menuBuilder.AddWidget(AddTagWidget, FText::FromString(""));
}

void MenuTool::MakeSubMenu(FMenuBuilder &menuBuilder)
{
	//menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand2);
	//menuBuilder.AddMenuEntry(MenuToolCommands::Get().MenuCommand3);	
}

void MenuTool::MenuCommand1()
{
	UE_LOG(LogClass, Log, TEXT("Existing"));
	InvokePhase(EPhaseType::Existing);
}

void MenuTool::MenuCommand2()
{
	UE_LOG(LogClass, Log, TEXT("BRT"));
	InvokePhase(EPhaseType::BRT);
}

void MenuTool::MenuCommand3()
{
	UE_LOG(LogClass, Log, TEXT("BRT No Ped"));
	InvokePhase(EPhaseType::BRTNoPed);
}

void MenuTool::MenuCommand4()
{
	UE_LOG(LogClass, Log, TEXT("LRT"));
	InvokePhase(EPhaseType::LRT);
}

void MenuTool::MenuCommand5()
{
	UE_LOG(LogClass, Log, TEXT("LRT No Ped"));
	InvokePhase(EPhaseType::LRTNoPed);
}

void MenuTool::MenuCommand6()
{
	UE_LOG(LogClass, Log, TEXT("DMU"));
	InvokePhase(EPhaseType::DMU);
}

void MenuTool::MenuCommand7()
{
	UE_LOG(LogClass, Log, TEXT("DMU No Ped"));
	InvokePhase(EPhaseType::DMUNoPed);

}


FReply MenuTool::AddTag()
{
	if (!TagToAdd.IsNone())
	{
		const FScopedTransaction Transaction(FText::FromString("Add Tag"));
		for (FSelectionIterator It(GEditor->GetSelectedActorIterator()); It; ++It)
		{
			AActor* Actor = static_cast<AActor*>(*It);
			if (!Actor->Tags.Contains(TagToAdd))
			{
				Actor->Modify();
				Actor->Tags.Add(TagToAdd);
			}
		}
	}

	return FReply::Handled();
}

FText MenuTool::GetTagToAddText() const
{
	return FText::FromName(TagToAdd);
}

void MenuTool::OnTagToAddTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	FString str = InText.ToString();
	TagToAdd = FName(*str.TrimStart()); 
}

void MenuTool::InvokePhase(EPhaseType InPhaseType)
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