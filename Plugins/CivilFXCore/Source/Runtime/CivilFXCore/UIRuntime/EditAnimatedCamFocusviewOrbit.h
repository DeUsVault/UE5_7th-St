// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "EditAnimatedCamFocusviewSubPanel.h"
#include "EditAnimatedCamFocusviewOrbit.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditAnimatedCamFocusviewOrbit : public UEditAnimatedCamFocusviewSubPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton) override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UCheckBox* EnableEditCheckBox;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class USpinBox* HeightSpinBox;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class USpinBox* RadiusSpinBox;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UTextableButton* CaptureLocationButton;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UTextableButton* ReCaptureLocationButton;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UTextableButton* GenerateViewButton;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UBackgroundBlur* OrbitPanelBackgroundBlur;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Panel Settings")
	TSubclassOf<AActor> OrbitObject;

private:
	float OrbitHeight;
	float OrbitRadius;
	bool bOrbitEditing;
	UFUNCTION()
	void OnOrbitCheckBoxChanged(bool bIsChecked);
	UFUNCTION()
	void OnOrbitHeightSpinBoxChanged(float InValue);
	UFUNCTION()
	void OnOrbitRadiusSpinBoxChanged(float InValue);
	UFUNCTION()
	void OnGenerateViewButtonClicked();
	UFUNCTION()
	void OnCaptureLocationButtonClicked();
	UFUNCTION()
	void OnReCaptureLocationButtonClicked();
	void GenerateLocationsAndRotations();

	TArray<FVector> GetEclipsePath(float XRadius, float YRadius, FVector Origin, float Height, float Theta = 0.f, int32 Resolution = 12);
};
