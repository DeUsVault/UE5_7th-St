// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
//#include "Widgets/Input/SComboButton.h"
#include "Widgets/Views/SListView.h"

#include "CivilFXCore/CommonCore/PhaseManager.h"

#include "MultiSelectionBoxPhasing.generated.h"


class SComboButton;
/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UMultiSelectionBoxPhasing : public UWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnOptionsSelected, TArray<FString>, SelectedOptions);
	FOnOptionsSelected OnOptionsSelected;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multi Selection")
	TArray<EPhaseType> PhaseOptions;

	void SetSelectedOptions(const TArray<FString>& Options);
	TArray<FString> GetSelectedOptions();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
private:
	TSharedPtr<SComboButton> CategoryComboButton;
	TSharedPtr<SListView<TSharedPtr<FText>>> CategoryListView;
	TArray<TSharedPtr<FText>> CategorySource;


	FText OnGetCategoryText() const;
	void OnCategoryTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit, FName VarName);
	TSharedRef<ITableRow> MakeCategoryViewWidget(TSharedPtr<FText> Item, const TSharedRef< STableViewBase >& OwnerTable);
	void OnCategorySelectionChanged(TSharedPtr<FText> ProposedSelection, ESelectInfo::Type /*SelectInfo*/);
	void ToggleSelectionOption(FText Selection);
	void RebuildCategorySource();

	const static FString CheckmarkSymbol;
	const static FString CheckmarkSymbolWithTab;
	const static FString PaddingSpace;

};
