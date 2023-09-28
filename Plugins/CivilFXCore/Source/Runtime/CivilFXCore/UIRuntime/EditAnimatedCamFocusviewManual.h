// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "EditAnimatedCamFocusviewSubPanel.h"
#include "EditAnimatedCameraManualButton.h"
#include "EditAnimatedCamFocusviewManual.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditAnimatedCamFocusviewManual : public UEditAnimatedCamFocusviewSubPanel
{
	GENERATED_BODY()
public:

    virtual void RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton) override;
    void AddChildToContainer(const FVector& InLocation, const FRotator& InRotation);

protected:
    UPROPERTY(EditInstanceOnly, Category = "Focusview Manual Panel Settings", meta = (BindWidget))
    class UScrollBox* ContainerScrollbox;
    UPROPERTY(EditInstanceOnly, Category = "Focusview Manual Panel Settings")
    TSubclassOf<UEditAnimatedCameraManualButton> EditAnimatedCameraManualButtonClass;

    virtual void OnTimelineSliderChanged(float InValue) override;

private:
    UFUNCTION()
    void OnRequestAddingNewChild(int32 Index);
    void OnRequestRemoveChild(int32 Index);
    void OnRequestRefreshChild(int32 Index);
    void UpdateChildrenCurrentIndex();

    UFUNCTION()
    void OnChildMainButtonClicked();

    UEditAnimatedCameraManualButton* CurrentSelectedButton;

    TArray<FVector> TempLocations;
    TArray<FRotator> TempRotations;
};
