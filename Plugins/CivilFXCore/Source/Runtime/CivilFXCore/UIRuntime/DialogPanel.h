// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogPanel.generated.h"

class USizeBox;

UCLASS()
class CIVILFXCORE_API UDialogPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetTitle(const FString& InTitle);
	void SetInputHint(const FString& InHint);

	//UUserWidget* CreateAndAddToViewport(UWorld* World, TSubclassOf<UDialogPanel> Class);

	UWidget* AddCustomWidget(TSubclassOf<UWidget> WidgetClass);

	DECLARE_DELEGATE_OneParam(FOnDialogConfirmed, FText)
	FOnDialogConfirmed& OnConfirmButtonClicked() { return DialogConfirmedDelegate; }

	DECLARE_DELEGATE(FOnDialogCanceled)
	FOnDialogCanceled& OnCancelButtonClicked() {return DialogCanceledDelegate;}

	virtual void NativeConstruct() override;

	


	static FText LastInputText;
	static UDialogPanel* CurrentPanel;
	/**
	 * 
		@return: Weak pointer because the panel is destroyed from within
	 */
	static TWeakObjectPtr<UDialogPanel> CreateAndAddToViewport(UWorld* WorldContext, const FString& InTitle=FString(), const FString& InHintText=FString());

protected:
	UPROPERTY(VisibleAnywhere, Category = "Dialog Panel Settings", meta = (BindWidget))
	class UTextBlock* TitleBox;

	UPROPERTY(VisibleAnywhere, Category="Dialog Panel Settings", meta=(BindWidget))
	class UEditableTextBox* InputTextBox;

	UPROPERTY(EditAnywhere, Category = "Dialog Panel Settings", meta = (BindWidget))
	class UTextableButton* ConfirmButton;

	UPROPERTY(EditAnywhere, Category = "Dialog Panel Settings", meta = (BindWidget))
	class UTextableButton* CancelButton;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Dialog Panel Settings", meta=(BindWidgetAnim))
	class UWidgetAnimation* PanelAnim;

	UPROPERTY(VisibleAnywhere, Category = "Dialog Panel Settings", meta = (BindWidgetOptional))
	USizeBox* OptionalExtraContentSizeBox;

	UFUNCTION()
	virtual void OnConfirm();
	UFUNCTION()
	void OnCancel();

	FOnDialogConfirmed DialogConfirmedDelegate;
	FOnDialogCanceled DialogCanceledDelegate;

	UFUNCTION()
	void OnInputBoxTextChanged(const FText& InText);

	void SetTitle(FText InText);
	void SetInputHint(FText InHint);
	virtual void RemovePanel();
};
