// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "ComboBoxCategory.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "UIRuntimeUtils.h"

#define LOCTEXT_NAMESPACE "UIRuntime"

#if WITH_EDITOR
const FText UComboBoxCategory::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}
#endif

void UComboBoxCategory::SetCategorySource(const TArray<FString>& InSource)
{
	TArray<FText> Source;
	Source.Reserve(InSource.Num());
	for (const FString& Src : InSource)
	{
		Source.Add(FText::FromString(Src));
	}
	SetCategorySource(Source);
}

void UComboBoxCategory::SetCategorySource(const TArray<FText>& InSource)
{
	TArray<TSharedPtr<FText>> TextSource;
	TextSource.Reserve(InSource.Num());
	for (const FText& Src : InSource)
	{
		TextSource.Add(MakeShared<FText>(Src));
	}

	if (TextSource.Num() > 0)
	{
		Text = *TextSource[0].Get();
	}

	CategorySource = MoveTemp(TextSource);
}

void UComboBoxCategory::AddCategorySourceItem(const FString& InItem)
{
	AddCategorySourceItem(FText::FromString(InItem));
}

void UComboBoxCategory::AddCategorySourceItem(const FText& InItem)
{
	bool bNewCategory = true;
	for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
	{
		bNewCategory &= !CategorySource[j].Get()->EqualTo(InItem);
	}
	if (bNewCategory)
	{
		CategorySource.Add(MakeShared<FText>(InItem));
	}
}

TSharedRef<SWidget> UComboBoxCategory::RebuildWidget()
{
	SAssignNew(CategoryComboButton, SComboButton)
	.ContentPadding(FMargin(0, 0, 5, 0))
	.ButtonContent()
	[
		SNew(SBorder)
		.Padding(FMargin(0, 0, 5, 0))
		[
			SNew(SEditableTextBox)
			.Text_UObject(this, &UComboBoxCategory::HandleGetCategoryText)
			.OnTextCommitted_UObject(this, &UComboBoxCategory::HandleCategoryTextCommitted)
			.SelectAllTextWhenFocused(true)
			.RevertTextOnEscape(true)
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
			.OnGenerateRow_UObject(this, &UComboBoxCategory::HandleGetCategoryViewWidget)
			.OnSelectionChanged_UObject(this, &UComboBoxCategory::HandleCategorySelectionChanged)
		]
	];
	return CategoryComboButton.ToSharedRef();
}

void UComboBoxCategory::UpdateCategorySource()
{
	if (CategoryListView)
	{
		CategoryListView->SetListItemsSource(CategorySource);
	}
}

FText UComboBoxCategory::HandleGetCategoryText() const
{
	if (OnGetText.IsBound())
	{ 
		return OnGetText.Execute();
	}
	return Text;
}

void UComboBoxCategory::HandleCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit)
{
	FText LCategoryName = FText::TrimPrecedingAndTrailing(NewText);
	Text = LCategoryName;

	OnTextCommitted.ExecuteIfBound(NewText);
	AddCategorySourceItem(LCategoryName);
}

TSharedRef<ITableRow> UComboBoxCategory::HandleGetCategoryViewWidget(TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(STextBlock).Text(*Item.Get())
	];
}

void UComboBoxCategory::HandleCategorySelectionChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type /*SelectInfo*/)
{
	if (NewSelection.IsValid())
	{
		FText Selection = *NewSelection.Get();

		CategoryListView->ClearSelection();
		CategoryComboButton->SetIsOpen(false);

		Text = Selection;

		OnTextCommitted.ExecuteIfBound(Selection);
	}
}

#undef LOCTEXT_NAMESPACE