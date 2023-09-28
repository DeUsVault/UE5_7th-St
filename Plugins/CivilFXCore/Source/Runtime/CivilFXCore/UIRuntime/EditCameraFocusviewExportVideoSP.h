// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "EditAnimatedCamFocusviewSubPanel.h"
#include "EditCameraFocusviewExportVideoSP.generated.h"

class UTextableButton;

UCLASS()
class CIVILFXCORE_API UEditCameraFocusviewExportVideoSP : public UEditAnimatedCamFocusviewSubPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void OnPanelVisible() override;
	virtual void OnPanelHidden() override;
	class UEditAnimatedCameraFocusviewPanel* ParentPanel;

protected:
	UPROPERTY(VisibleInstanceOnly, meta=(BindWidget))
	class UButton* BackButton;
	/** Video */
	UPROPERTY(VisibleInstanceOnly, meta = (BindWidget))
	UTextableButton* Video4KButton;
	UPROPERTY(VisibleInstanceOnly, meta = (BindWidget))
	UTextableButton* Video1080PUltraButton;
	UPROPERTY(VisibleInstanceOnly, meta = (BindWidget))
	UTextableButton* Video1080PButton;
private:

	class URenderingPanel* RenderingPanel;
	class UEditAnimatedCameraPanel* OuterEditPanel;

	UFUNCTION()
	void HandleBackButtonClicked();
	UFUNCTION()
	void Handle4KButtonClicked();
	UFUNCTION()
	void Handle1080PUltraButtonClick();
	UFUNCTION()
	void Handle1080PButtonClicked();

	UFUNCTION()
	void HandleCameraPreviewEnded();
};
