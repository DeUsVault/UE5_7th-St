// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimatedCameraNodeData.h"
#include "EditAnimatedCamFocusviewSubPanel.generated.h"

class UEditAnimatedCameraOverviewButton;

UCLASS(Abstract)
class CIVILFXCORE_API UEditAnimatedCamFocusviewSubPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton);

	void SetTimelineSliderValue(float InValue);
	float GetTimelineSliderValue() const;

	UFUNCTION()
	virtual void OnTimelineSliderChanged(float InValue);
	UFUNCTION()
	virtual void OnTimelineStartButtonClicked();
	virtual void OnTimelineEndButtonClicked() PURE_VIRTUAL(UEditAnimatedCamFocusviewSubPanel::OnTimelineEndButtonClicked,);

	virtual void OnPanelVisible();
	virtual void OnPanelHidden();

protected:
	UPROPERTY(EditInstanceOnly, Category = "Focusview Manual Panel Settings", meta = (BindWidget))
	class USlider* TimelineSlider;

	UEditAnimatedCameraOverviewButton* EditingButton;
	TSharedPtr<FAnimatedCameraNodeData> CameraNodeData;
	class ACivilFXPawn* PlayerPawn;



};
