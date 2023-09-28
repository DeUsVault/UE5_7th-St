// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "DialogPanel.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "TextableButton.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"

FText UDialogPanel::LastInputText = FText::GetEmpty();
UDialogPanel* UDialogPanel::CurrentPanel = nullptr;

void UDialogPanel::NativeConstruct()
{
	ConfirmButton->GetButton()->OnClicked.AddDynamic(this, &UDialogPanel::OnConfirm);
	CancelButton->GetButton()->OnClicked.AddDynamic(this, &UDialogPanel::OnCancel);

	InputTextBox->OnTextChanged.AddDynamic(this, &UDialogPanel::OnInputBoxTextChanged);
	InputTextBox->SetText(LastInputText);
	InputTextBox->SetKeyboardFocus();
}

void UDialogPanel::OnInputBoxTextChanged(const FText& InText)
{
	FString TempString = InText.ToString();
	if (TempString.Len() > 18)
	{
		PlayAnimation(PanelAnim);
		TempString = TempString.Left(18);
		InputTextBox->SetText(FText::FromString(TempString));
	}
}

void UDialogPanel::OnConfirm()
{
	LastInputText = InputTextBox->GetText();

	if (LastInputText.IsEmpty())
	{
		PlayAnimation(PanelAnim);
		ConfirmButton->OnDeselected();
		return;
	}

	DialogConfirmedDelegate.ExecuteIfBound(LastInputText);
	RemovePanel();
}

void UDialogPanel::OnCancel()
{
	DialogCanceledDelegate.ExecuteIfBound();
	RemovePanel();
}

void UDialogPanel::SetTitle(const FString& InTitle)
{
	SetTitle(FText::FromString(InTitle));
}

void UDialogPanel::SetInputHint(const FString& InTitle)
{
	SetInputHint(FText::FromString(InTitle));
}


UWidget* UDialogPanel::AddCustomWidget(TSubclassOf<UWidget> WidgetClass)
{
	if (!OptionalExtraContentSizeBox)
	{
		return nullptr;
	}

	UWidget* CreatedWidget = WidgetTree->ConstructWidget<UWidget>(WidgetClass);
	if (CreatedWidget)
	{
		OptionalExtraContentSizeBox->AddChild(CreatedWidget);
		OptionalExtraContentSizeBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	return CreatedWidget;
}

void UDialogPanel::SetTitle(FText InTitle)
{
	TitleBox->SetText(InTitle);
}

void UDialogPanel::SetInputHint(FText InHint)
{
	InputTextBox->SetHintText(InHint);
}

void UDialogPanel::RemovePanel()
{
	CurrentPanel = nullptr;
	RemoveFromViewport();
}

TWeakObjectPtr<UDialogPanel> UDialogPanel::CreateAndAddToViewport(UWorld* WorldContext, const FString& InTitle /*= FString()*/, const FString& InHintText /*= FString()*/)
{
	if (IsValid(CurrentPanel))
	{
		CurrentPanel->OnCancel();
	}

	TSubclassOf<UDialogPanel> PanelClass = LoadClass<UDialogPanel>(nullptr, TEXT("WidgetBlueprint'/CivilFXCore/UI_BPs/BP_DialogPanel.BP_DialogPanel_C'"));
	check(PanelClass);
	UDialogPanel* Widget = Cast<UDialogPanel>(CreateWidget(WorldContext, PanelClass));
	check(Widget);
	Widget->SetTitle(InTitle);
	Widget->SetInputHint(InHintText);
	Widget->AddToViewport();

	CurrentPanel = Widget;

	return Widget;
}
