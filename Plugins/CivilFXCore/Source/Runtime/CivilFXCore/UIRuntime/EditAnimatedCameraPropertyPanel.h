// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimatedCameraNodeData.h"
#include "EditAnimatedCameraPropertyPanel.generated.h"

class UEditAnimatedCameraOverviewButton;
class UEditAnimatedCameraFocusviewPanel;

UCLASS()
class CIVILFXCORE_API UEditAnimatedCameraPropertyPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetEnable();
	void SetDisable();
	void RefreshPanel(UEditAnimatedCameraOverviewButton* InCurrentEditingButton);
	void SetFocusviewPanel(UEditAnimatedCameraFocusviewPanel* InFocusviewPanel);

	void SetComboBoxCategorySource(const TArray<FString>& InSource);

	virtual void NativeConstruct() override;
protected:
	UPROPERTY(EditInstanceOnly, Category="Properties Panel Settings", meta=(BindWidget))
	UWidget* MainBackgroundBlur;

	UPROPERTY(EditInstanceOnly, Category = "Properties Panel Settings", meta = (BindWidget))
	class UEditableTextBox* CameraNameInputBox;

	UPROPERTY(EditInstanceOnly, Category = "Properties Panel Settings", meta = (BindWidget))
	class UComboBoxString* TypeComboBox;

	UPROPERTY(EditInstanceOnly, Category = "Properties Panel Settings", meta = (BindWidget))
	class UMultiSelectionBoxPhasing* PhaseMultiSelectionBox;

	

	UPROPERTY(EditInstanceOnly, Category = "Properties Panel Settings", meta = (BindWidget))
	class UEditableText* MinutesInputText;
	UPROPERTY(EditInstanceOnly, Category = "Properties Panel Settings", meta = (BindWidget))
	class UEditableText* SecondsInputText;

	UPROPERTY(EditInstanceOnly, Category = "Properties Panel Settings", meta = (BindWidget))
	class UComboBoxCategory* CategoryBox;

private:
	UEditAnimatedCameraOverviewButton* CurrentEditingButton;
	TSharedPtr<FAnimatedCameraNodeData> CameraNodeData;

	static const FText DefaultCameraName;

	UFUNCTION()
	void OnCameraNameTextChanged(const FText& NewText);
	UFUNCTION()
	void OnCameraNameTextCommited(const FText& NewText, ETextCommit::Type CommitedType);
	UFUNCTION()
	void OnTypeComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectType);

	UFUNCTION()
	void HandlePhaseOptionsSelected(TArray<FString> SelectedOptions);

	UFUNCTION()
	void OnDurationTextInputCommited(const FText& InText, ETextCommit::Type CommitedType);
	float GetDurationFromText(const FText& MinutesText, const FText& SecondsText) const;
	void GetTextFromDuration(float InDuration, FText& OutMinutesText, FText& OutSecondsText) const;

	FText HandleAnimatedCategoryBoxGetText();
	void HandleAnimatedCategoryBoxTextCommitted(const FText& NewText);

	UEditAnimatedCameraFocusviewPanel* FocusviewPanel;
};
