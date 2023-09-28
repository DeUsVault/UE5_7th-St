// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraFocusviewPanel.h"
#include "EditCameraFocusviewExportVideoSP.h"

#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"

#include "EditAnimatedCameraOverviewButton.h"

#include "EditAnimatedCamFocusviewOrbit.h"
#include "EditAnimatedCamFocusviewManual.h"
#include "EditAnimatedCamFocusviewMovement.h"

void UEditAnimatedCameraFocusviewPanel::NativeConstruct()
{
	PlayButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraFocusviewPanel::OnPlayButtonClicked);
	PauseButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraFocusviewPanel::OnPauseButtonClicked);
	RestartButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraFocusviewPanel::OnRestartButtonClicked);
	EndButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraFocusviewPanel::OnEndButtonClicked);

	ExportVideoButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraFocusviewPanel::HandleExportVideoButtonClicked);
}

void UEditAnimatedCameraFocusviewPanel::SwitchPanel(EFocusviewPanel NewPanel)
{
	//GLog->Log("Executing Hidden Panel");
	if (EdittingPanel)
	{
		EdittingPanel->OnPanelHidden();
	}

	switch (NewPanel)
	{
	case EFocusviewPanel::None:
		MainSwitcher->SetActiveWidgetIndex(0);
		EdittingPanel = nullptr;
		break;
	case EFocusviewPanel::Orbit:
		MainSwitcher->SetActiveWidgetIndex(1);
		EdittingPanel = OrbitPanel;
		break;
	case EFocusviewPanel::Manual:
		MainSwitcher->SetActiveWidgetIndex(2);
		EdittingPanel = ManualPanel;
		break;
	case EFocusviewPanel::Movement:
		MainSwitcher->SetActiveWidgetIndex(3);
		EdittingPanel = MovementPanel;
		break;
	case EFocusviewPanel::ExportVideo:
		MainSwitcher->SetActiveWidgetIndex(4);
		EdittingPanel = Cast<UEditAnimatedCamFocusviewSubPanel>(MainSwitcher->GetActiveWidget());
		Cast<UEditCameraFocusviewExportVideoSP>(EdittingPanel)->ParentPanel = this;
		break;
	}

	//GLog->Log("Executing Refresh Panel");
	//Clear any active slider
	OnPauseButtonClicked();
	if (EdittingPanel)
	{
		EdittingPanel->RefreshPanel(EditingButton);
		EdittingPanel->SetTimelineSliderValue(0.0);
		EdittingPanel->OnPanelVisible();
	}
}


void UEditAnimatedCameraFocusviewPanel::OnPlayButtonClicked()
{
	if (!EditingButton || !EdittingPanel)
	{
		return;
	}

	PlayButton->SetVisibility(ESlateVisibility::Hidden);
	PauseButton->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEditAnimatedCameraFocusviewPanel::SimulatedTick);
	bShouldRescheduleTick = true;
}

void UEditAnimatedCameraFocusviewPanel::SimulatedTick()
{
	float SliderValue = EdittingPanel->GetTimelineSliderValue();
	if (SliderValue >= 1.f)
	{
		OnPauseButtonClicked();
		return;
	}


	static const FVector2D ZeroOneVector2D = FVector2D(0, 1);
	FVector2D DurationInput(0, EditingButton->GetCameraNodeData()->Duration);

	CurrentDuration = FMath::GetMappedRangeValueClamped(ZeroOneVector2D, DurationInput, SliderValue);

	CurrentDuration += GetWorld()->GetDeltaSeconds();
	SliderValue = FMath::GetMappedRangeValueClamped(DurationInput, ZeroOneVector2D, CurrentDuration);

	EdittingPanel->SetTimelineSliderValue(SliderValue);

	//reschedule itself
	if (bShouldRescheduleTick)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEditAnimatedCameraFocusviewPanel::SimulatedTick);
	}
}

void UEditAnimatedCameraFocusviewPanel::OnPauseButtonClicked()
{
	PlayButton->SetVisibility(ESlateVisibility::Visible);
	PauseButton->SetVisibility(ESlateVisibility::Hidden);

	//clear the timer
	bShouldRescheduleTick = false;

	OnCameraPreviewEnded.Broadcast();
}

void UEditAnimatedCameraFocusviewPanel::OnRestartButtonClicked()
{
	EdittingPanel->SetTimelineSliderValue(0);
}

void UEditAnimatedCameraFocusviewPanel::OnEndButtonClicked()
{
	EdittingPanel->SetTimelineSliderValue(1.0f);
}

void UEditAnimatedCameraFocusviewPanel::HandleExportVideoButtonClicked()
{
	SwitchPanel(EFocusviewPanel::ExportVideo);

	PreviewControlPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UEditAnimatedCameraFocusviewPanel::RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton)
{
	EditingButton = InEditingButton;
	
	//maybe refresh the name here?
	OnCameraNameChangedFromPropertyPanel(FText::FromString(EditingButton->GetCameraNodeData()->CameraName));
}

void UEditAnimatedCameraFocusviewPanel::OnCameraNameChangedFromPropertyPanel(FText InText)
{
	//GLog->Log("Executing in here too?");
	CameraNameTextBlock->SetText(InText);
}

void UEditAnimatedCameraFocusviewPanel::OnCameraTypeChangedFromPropertyPanel(FString InName)
{
	if (InName.Equals(TEXT("Orbit")))
	{
		SwitchPanel(EFocusviewPanel::Orbit);
	}

	else if (InName.Equals(TEXT("Manual")))
	{
		SwitchPanel(EFocusviewPanel::Manual);
	}
	else if (InName.Equals(TEXT("Movement")))
	{
		SwitchPanel(EFocusviewPanel::Movement);
	}
}

void UEditAnimatedCameraFocusviewPanel::OnExited()
{
	OnPauseButtonClicked();
}

void UEditAnimatedCameraFocusviewPanel::OnBeginRecording()
{
	OnRestartButtonClicked();
	OnPlayButtonClicked();
}

void UEditAnimatedCameraFocusviewPanel::ShowPreviewControlPanel()
{
	PreviewControlPanel->SetVisibility(ESlateVisibility::Visible);
}
