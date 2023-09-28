// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraPropertyPanel.h"

#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"

#include "EditAnimatedCameraOverviewButton.h"
#include "EditAnimatedCameraFocusviewPanel.h"
#include "MultiSelectionBoxPhasing.h"
#include "ComboBoxCategory.h"

const FText UEditAnimatedCameraPropertyPanel::DefaultCameraName = FText::FromString(TEXT("<Camera Name>"));

void UEditAnimatedCameraPropertyPanel::NativeConstruct()
{
	CameraNameInputBox->SelectAllTextWhenFocused = true;
	CameraNameInputBox->SynchronizeProperties();
	CameraNameInputBox->OnTextChanged.AddDynamic(this, &UEditAnimatedCameraPropertyPanel::OnCameraNameTextChanged);
	CameraNameInputBox->OnTextCommitted.AddDynamic(this, &UEditAnimatedCameraPropertyPanel::OnCameraNameTextCommited);

	TypeComboBox->OnSelectionChanged.AddDynamic(this, &UEditAnimatedCameraPropertyPanel::OnTypeComboBoxSelectionChanged);

	MinutesInputText->SelectAllTextWhenFocused = true;
	MinutesInputText->SynchronizeProperties();
	MinutesInputText->OnTextCommitted.AddDynamic(this, &UEditAnimatedCameraPropertyPanel::OnDurationTextInputCommited);
	SecondsInputText->SelectAllTextWhenFocused = true;
	SecondsInputText->SynchronizeProperties();
	SecondsInputText->OnTextCommitted.AddDynamic(this, &UEditAnimatedCameraPropertyPanel::OnDurationTextInputCommited);

	CategoryBox->OnGetText.BindUObject(this, &UEditAnimatedCameraPropertyPanel::HandleAnimatedCategoryBoxGetText);
	CategoryBox->OnTextCommitted.BindUObject(this, &UEditAnimatedCameraPropertyPanel::HandleAnimatedCategoryBoxTextCommitted);

	PhaseMultiSelectionBox->OnOptionsSelected.BindDynamic(this, &UEditAnimatedCameraPropertyPanel::HandlePhaseOptionsSelected);
}

void UEditAnimatedCameraPropertyPanel::SetEnable()
{
	MainBackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
}

void UEditAnimatedCameraPropertyPanel::SetDisable()
{
	MainBackgroundBlur->SetVisibility(ESlateVisibility::Visible);
}

void UEditAnimatedCameraPropertyPanel::RefreshPanel(UEditAnimatedCameraOverviewButton* InCurrentEditingButton)
{
	if (CurrentEditingButton)
	{
		CurrentEditingButton->UpdateCameraNodeData();
	}
	

	FocusviewPanel->RefreshPanel(InCurrentEditingButton);

	CurrentEditingButton = InCurrentEditingButton;
	CameraNodeData = CurrentEditingButton->GetCameraNodeData();

	CameraNameInputBox->SetText(CurrentEditingButton->GetCameraName());

	/**/
	TypeComboBox->SetSelectedOption(InCurrentEditingButton->GetCameraNodeData()->CameraType);
	TypeComboBox->OnSelectionChanged.Broadcast(InCurrentEditingButton->GetCameraNodeData()->CameraType, ESelectInfo::Direct);

	/*Set duration boxes to passed in values*/
	FText MinutesText;
	FText SecondsText;
	GetTextFromDuration(InCurrentEditingButton->GetCameraNodeData()->Duration, MinutesText, SecondsText);
	MinutesInputText->SetText(MinutesText);
	SecondsInputText->SetText(SecondsText);


	PhaseMultiSelectionBox->SetSelectedOptions(CameraNodeData->Phases);
}


void UEditAnimatedCameraPropertyPanel::SetFocusviewPanel(UEditAnimatedCameraFocusviewPanel* InFocusviewPanel)
{
	FocusviewPanel = InFocusviewPanel;
}

void UEditAnimatedCameraPropertyPanel::SetComboBoxCategorySource(const TArray<FString>& InSource)
{
	CategoryBox->SetCategorySource(InSource);
}

void UEditAnimatedCameraPropertyPanel::OnCameraNameTextChanged(const FText& NewText)
{
	FString TempString = NewText.ToString();
	if (TempString.Len() > 18)
	{
		TempString = TempString.Left(18);
		CameraNameInputBox->SetText(FText::FromString(TempString));
	}

}

void UEditAnimatedCameraPropertyPanel::OnCameraNameTextCommited(const FText& NewText, ETextCommit::Type InType)
{
	FText NewNameText = NewText.IsEmpty() ? DefaultCameraName : NewText;

	CurrentEditingButton->SetCameraName(NewNameText);
	CameraNodeData->CameraName = NewNameText.ToString();

	FocusviewPanel->OnCameraNameChangedFromPropertyPanel(NewNameText);
}

void UEditAnimatedCameraPropertyPanel::OnTypeComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type)
{
	CameraNodeData->CameraType = SelectedItem;
	FocusviewPanel->OnCameraTypeChangedFromPropertyPanel(SelectedItem);
}

void UEditAnimatedCameraPropertyPanel::HandlePhaseOptionsSelected(TArray<FString> SelectedOptions)
{
	CameraNodeData->Phases = SelectedOptions;
}

void UEditAnimatedCameraPropertyPanel::OnDurationTextInputCommited(const FText& InText, ETextCommit::Type CommitedType)
{
	FString TempString = InText.ToString();

	//replacing +- to invalid numeric character ('A')
	TempString.ReplaceInline(TEXT("+-"), TEXT("A"));

	if (TempString.IsNumeric())
	{
		CameraNodeData->Duration = GetDurationFromText(MinutesInputText->GetText(), SecondsInputText->GetText());
	}
	FText MinutesText;
	FText SecondsText;
	GetTextFromDuration(CameraNodeData->Duration, MinutesText, SecondsText);
	MinutesInputText->SetText(MinutesText);
	SecondsInputText->SetText(SecondsText);
}

float UEditAnimatedCameraPropertyPanel::GetDurationFromText(const FText& MinutesText, const FText& SecondsText) const
{
	int32 Minutes = FCString::Atoi(*MinutesText.ToString());
	int32 Seconds = FCString::Atoi(*SecondsText.ToString());
	return Minutes * 60.0f + Seconds;
}

void UEditAnimatedCameraPropertyPanel::GetTextFromDuration(float InDuration, FText& OutMinutesText, FText& OutSecondsText) const
{
	int32 DurationAsInt = FMath::CeilToInt(InDuration);
	int32 Minutes = DurationAsInt / 60; //integer math
	int32 Seconds = DurationAsInt % 60;
	OutMinutesText = FText::FromString(FString::Printf(TEXT("%02d"), Minutes));
	OutSecondsText = FText::FromString(FString::Printf(TEXT("%02d"), Seconds));
}

FText UEditAnimatedCameraPropertyPanel::HandleAnimatedCategoryBoxGetText()
{
	if (CameraNodeData.IsValid())
	{
		return FText::FromString(CameraNodeData->Category);
	}
	return FText();
}

void UEditAnimatedCameraPropertyPanel::HandleAnimatedCategoryBoxTextCommitted(const FText& NewText)
{
	if (CameraNodeData.IsValid())
	{
		CameraNodeData->Category = NewText.ToString();
	}
}
