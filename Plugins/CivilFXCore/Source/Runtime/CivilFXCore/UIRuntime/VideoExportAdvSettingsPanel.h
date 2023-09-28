// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ExpandableArea.h"
#include "VideoExportAdvSettingsPanel.generated.h"


UCLASS()
class CIVILFXCORE_API UVideoExportAdvSettingsPanel : public UUserWidget
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;

	FOnExpandableAreaExpansionChanged& OnExpandableAreaExpansionChanged();

	bool GetExceededRecordingDuration(float& OutDuration) const;

protected:
	UPROPERTY(VisibleInstanceOnly, meta = (BindWidget))
	UExpandableArea* VideoAdvSettingsArea;

	UPROPERTY(VisibleInstanceOnly, meta=(BindWidget))
	class UCheckBox* AutoStopCheckbox;
	UPROPERTY(VisibleInstanceOnly, meta=(BindWidget))
	class UDurationTime* DurationTime;

private:
	UFUNCTION()
	void HandleCheckBoxStateChanged(bool bIsChecked);
	UFUNCTION()
	void DeferShowingExpandableAreContent(UExpandableArea* Area, bool bIsExpanded);
	FTimerHandle TimerHandle;
};
