// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "MultiSelectionBoxPhasing.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "UIRuntime"

const FString UMultiSelectionBoxPhasing::CheckmarkSymbol = UTF16_TO_TCHAR(u"\x2713");
const FString UMultiSelectionBoxPhasing::CheckmarkSymbolWithTab = FString::Format(TEXT("{0}{1}"),{CheckmarkSymbol, TEXT("\t")});
const FString UMultiSelectionBoxPhasing::PaddingSpace = TEXT("\t\t");



void UMultiSelectionBoxPhasing::SetSelectedOptions(const TArray<FString>& Options)
{
	RebuildCategorySource();

	for (const FString& Option : Options)
	{
		for (int32 j = 0; j < CategorySource.Num(); ++j)
		{
			const FString& CategoryString = CategorySource[j].Get()->ToString();
			if (CategoryString.Contains(Option))
			{
				CategorySource[j] = MakeShareable(new FText(FText::FromString(CategoryString.Replace(*PaddingSpace, *CheckmarkSymbolWithTab))));
				break;
			}
		}
	}
}

TArray<FString> UMultiSelectionBoxPhasing::GetSelectedOptions()
{
	TArray<FString> SelectedOptions;
	for (int32 j = 0; j < CategorySource.Num(); ++j)
	{
		const FString& CategoryString = CategorySource[j].Get()->ToString();
		if (CategoryString.Contains(CheckmarkSymbol))
		{
			FString Temp = CategoryString.Replace(*CheckmarkSymbol, TEXT("")).TrimStartAndEnd();
			SelectedOptions.Add(Temp);
		}
	}
	return SelectedOptions;
}

TSharedRef<SWidget> UMultiSelectionBoxPhasing::RebuildWidget()
{
	RebuildCategorySource();

	//this variable is not used
	//TODO: remove it
	const FName CachedVariableName = TEXT("<Var>");

	SAssignNew(CategoryComboButton, SComboButton)
		.ContentPadding(FMargin(0, 0, 5, 0))
		.ButtonContent()
		[
			SNew(SBorder)
			.Padding(FMargin(0, 0, 5, 0))
			[
				/*
				SNew(SEditableTextBox)
				//.Text(this, &UComboBoxCategory::OnGetCategoryText)
				.Text_UObject(this, &UMultiSelectionBoxPhasing::OnGetCategoryText)
				.OnTextCommitted_UObject(this, &UMultiSelectionBoxPhasing::OnCategoryTextCommitted, CachedVariableName)
				.SelectAllTextWhenFocused(true)
				.RevertTextOnEscape(true)
				*/
				SNew(STextBlock)
				.Text_UObject(this, &UMultiSelectionBoxPhasing::OnGetCategoryText)
			]
		]
		.MenuContent()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(400.0f)
			[
				SAssignNew(CategoryListView, SListView<TSharedPtr<FText>>)
				.ListItemsSource(&CategorySource)
				.OnGenerateRow_UObject(this, &UMultiSelectionBoxPhasing::MakeCategoryViewWidget)
				.OnSelectionChanged_UObject(this, &UMultiSelectionBoxPhasing::OnCategorySelectionChanged)
			]
		];
	return CategoryComboButton.ToSharedRef();
}

FText UMultiSelectionBoxPhasing::OnGetCategoryText() const
{
	FText Text = FText::GetEmpty();

	int32 SelectionCount = 0;
	int32 SelectionIndex = 0;
	for (int32 j = 0; j < CategorySource.Num(); ++j)
	{
		const FString& CategoryString = CategorySource[j].Get()->ToString();
		if (CategoryString.Contains(CheckmarkSymbol))
		{
			++SelectionCount;
			SelectionIndex = j;
		}
	}

	if (SelectionCount > 1)
	{
		Text = FText::FromString(TEXT("Mixed..."));
	}
	else
	{
		Text = FText::FromString(CategorySource[SelectionIndex].Get()->ToString().Replace(*CheckmarkSymbolWithTab, TEXT("")));
	}
	return Text;
}

void UMultiSelectionBoxPhasing::OnCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit, FName VarName)
{
	FText LCategoryName = FText::TrimPrecedingAndTrailing(NewText);
	//PopulateCategory(LCategoryName);
}

TSharedRef<ITableRow> UMultiSelectionBoxPhasing::MakeCategoryViewWidget(TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable)
{
	//FString Str = UTF16_TO_TCHAR(u"\x2713");
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			SNew(STextBlock).Text(*Item.Get())
			//SNew(STextBlock).Text(FText::FromString(Str))
		];
}

void UMultiSelectionBoxPhasing::OnCategorySelectionChanged(TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/)
{
	if (ProposedSelection.IsValid())
	{
		FText Selection = *ProposedSelection.Get();

		ToggleSelectionOption(Selection);

		CategoryListView->ClearSelection();
		CategoryListView->RebuildList();
		
		OnOptionsSelected.ExecuteIfBound(GetSelectedOptions());
	}
}


void UMultiSelectionBoxPhasing::ToggleSelectionOption(FText Selection)
{
	for (int32 j = 0; j < CategorySource.Num(); ++j)
	{
		const FString& CategoryString = CategorySource[j].Get()->ToString();
		if (CategoryString.Contains(Selection.ToString()))
		{
			if (CategoryString.Contains(CheckmarkSymbol))
			{
				CategorySource[j] = MakeShareable(new FText(FText::FromString(CategoryString.Replace(*CheckmarkSymbolWithTab, *PaddingSpace))));
			}
			else
			{
				CategorySource[j] = MakeShareable(new FText(FText::FromString(CategoryString.Replace(*PaddingSpace, *CheckmarkSymbolWithTab))));
			}
		}
	}
}

void UMultiSelectionBoxPhasing::RebuildCategorySource()
{
	CategorySource.Empty();
	for (EPhaseType Option : PhaseOptions)
	{
		FText TextValue = UEnum::GetDisplayValueAsText(Option);

#if WITH_EDITOR
		FString ValueName = TextValue.ToString();
#else
		FString ValueName = FName::NameToDisplayString(TextValue.ToString(), false);
#endif

		FText CategoryText = FText::Format(FText::FromString("{0}{1}"), FText::FromString(PaddingSpace), FText::FromString(ValueName));
		CategorySource.Add(MakeShareable(new FText(CategoryText)));
	}

}

#if WITH_EDITOR
const FText UMultiSelectionBoxPhasing::GetPaletteCategory()
{
	return LOCTEXT("Primitive", "Primitive");
}
#endif


#undef LOCTEXT_NAMESPACE
