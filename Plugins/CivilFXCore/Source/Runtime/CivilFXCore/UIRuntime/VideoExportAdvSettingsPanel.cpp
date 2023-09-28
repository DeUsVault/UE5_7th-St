// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "VideoExportAdvSettingsPanel.h"

#include "DurationTime.h"

#include "Components/CheckBox.h"


FOnExpandableAreaExpansionChanged& UVideoExportAdvSettingsPanel::OnExpandableAreaExpansionChanged()
{
	return VideoAdvSettingsArea->OnExpansionChanged;
}

bool UVideoExportAdvSettingsPanel::GetExceededRecordingDuration(float& OutDuration) const
{
	if (AutoStopCheckbox->GetCheckedState() == ECheckBoxState::Checked)
	{
		OutDuration = DurationTime->GetDuration();
		return true;
	}
	return false;
}

void UVideoExportAdvSettingsPanel::NativeConstruct()
{
	AutoStopCheckbox->OnCheckStateChanged.AddDynamic(this, &UVideoExportAdvSettingsPanel::HandleCheckBoxStateChanged);
	DurationTime->SetInteractable(AutoStopCheckbox->GetCheckedState() == ECheckBoxState::Checked);

	VideoAdvSettingsArea->OnExpansionChanged.AddDynamic(this, &UVideoExportAdvSettingsPanel::DeferShowingExpandableAreContent);
}

void UVideoExportAdvSettingsPanel::HandleCheckBoxStateChanged(bool bIsChecked)
{
	DurationTime->SetInteractable(bIsChecked);
}

void UVideoExportAdvSettingsPanel::DeferShowingExpandableAreContent(UExpandableArea*, bool bIsExpanded)
{
	UWidget* Content = VideoAdvSettingsArea->GetContentForSlot(TEXT("Body"));
	Content->SetVisibility(ESlateVisibility::Hidden);

	if (bIsExpanded)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Content]()
		{
			Content->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}, 0.1f, false);
	}
}
