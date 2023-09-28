// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SListView.h"
#include "ComboBoxCategory.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UComboBoxCategory : public UWidget
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
		virtual const FText GetPaletteCategory() override;
#endif

	FText Text;

	DECLARE_DELEGATE_RetVal(FText, FOnGetCategoryText);
	FOnGetCategoryText OnGetText;

	DECLARE_DELEGATE_OneParam(FOnTextCommitted, const FText&);
	FOnTextCommitted OnTextCommitted;

	void SetCategorySource(const TArray<FString>& InSource);
	void SetCategorySource(const TArray<FText>& InSource);
	void AddCategorySourceItem(const FString& InItem);
	void AddCategorySourceItem(const FText& InItem);

	//void PopulateCategory(FText NewCategory);
	//void PopulateCategory(TArray<FText> NewCategories);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void UpdateCategorySource();
private:
	TSharedPtr<SComboButton> CategoryComboButton;
	TSharedPtr<SListView<TSharedPtr<FText>>> CategoryListView;
	TArray<TSharedPtr<FText>> CategorySource;

	FText HandleGetCategoryText() const;
	void HandleCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit);
	TSharedRef<ITableRow> HandleGetCategoryViewWidget(TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable);
	void HandleCategorySelectionChanged(TSharedPtr<FText> NewSelection, ESelectInfo::Type /*SelectInfo*/);
};
