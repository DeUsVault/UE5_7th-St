// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCamFocusviewSubPanel.h"

#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "EditAnimatedCameraOverviewButton.h"
#include "Components/Slider.h"

void UEditAnimatedCamFocusviewSubPanel::NativeConstruct()
{
	PlayerPawn = Cast<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	TimelineSlider->OnValueChanged.AddDynamic(this, &UEditAnimatedCamFocusviewSubPanel::OnTimelineSliderChanged);
}

void UEditAnimatedCamFocusviewSubPanel::RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton)
{
	if (InEditingButton == EditingButton)
	{
		return;
	}
	EditingButton = InEditingButton;
	CameraNodeData = EditingButton->GetCameraNodeData();

	/** Rebuild Camera Data here */
	const float HeightOffset = CameraNodeData->IsMovementType() ? CameraNodeData->Height : 0;
	PlayerPawn->OnBeginCameraMovement(CameraNodeData->Locations, CameraNodeData->Rotations, CameraNodeData->Duration, HeightOffset, true);
}

void UEditAnimatedCamFocusviewSubPanel::SetTimelineSliderValue(float InValue)
{
	TimelineSlider->OnValueChanged.Broadcast(InValue);
	TimelineSlider->SetValue(InValue);
}

float UEditAnimatedCamFocusviewSubPanel::GetTimelineSliderValue() const
{
	return TimelineSlider->GetValue();
}

void UEditAnimatedCamFocusviewSubPanel::OnTimelineSliderChanged(float InValue)
{
	PlayerPawn->OnBeginPreviewAtTimeline(InValue);
}

void UEditAnimatedCamFocusviewSubPanel::OnTimelineStartButtonClicked()
{

}

void UEditAnimatedCamFocusviewSubPanel::OnPanelVisible()
{

}

void UEditAnimatedCamFocusviewSubPanel::OnPanelHidden()
{

}