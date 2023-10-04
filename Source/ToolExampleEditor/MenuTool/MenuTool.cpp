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
		UI_COMMAND(MenuCommand1, "Proposed", "Proposed phase", EUserInterfaceActionType::Button, FInputGesture());
	}

public:
	TSharedPtr<FUICommandInfo> MenuCommand1;
};

void MenuTool::MapCommands()
{
	const auto& Commands = MenuToolCommands::Get();

	CommandList->MapAction(
		Commands.MenuCommand1,
		FExecuteAction::CreateSP(this, &MenuTool::MenuCommand1),
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
	UE_LOG(LogClass, Log, TEXT("Proposed"));
	InvokePhase(EPhaseType::Proposed);
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