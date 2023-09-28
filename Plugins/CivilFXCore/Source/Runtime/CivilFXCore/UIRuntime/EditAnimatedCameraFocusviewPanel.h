// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimatedCameraNodeData.h"
#include "EditAnimatedCameraFocusviewPanel.generated.h"

class UEditAnimatedCameraOverviewButton;

enum class EFocusviewPanel : uint8
{
	None,
	Orbit,
	Manual,
	Movement,
	ExportVideo
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraPreviewEnded);

UCLASS()
class CIVILFXCORE_API UEditAnimatedCameraFocusviewPanel : public UUserWidget
{
	GENERATED_BODY()
public:

	FOnCameraPreviewEnded OnCameraPreviewEnded;
	
	virtual void NativeConstruct() override;

	void RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton);

	/** Orbit/Eclipse Panel */
	void OnCameraNameChangedFromPropertyPanel(FText InText);
	void OnCameraTypeChangedFromPropertyPanel(FString InName);

	void OnExited();
	void OnBeginRecording();
	void ShowPreviewControlPanel();

	/** Orbit/Eclipse Panel */
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
		class UEditAnimatedCamFocusviewOrbit* OrbitPanel;

	/** Manual Panel */
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
		class UEditAnimatedCamFocusviewManual* ManualPanel;

	/** Manual Panel */
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
		class UEditAnimatedCamFocusviewMovement* MovementPanel;


protected:
	UPROPERTY(VisibleInstanceOnly, Category="Focusview Panel Settings", meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UTextBlock* CameraNameTextBlock;

	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UOverlay* PreviewControlPanel;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UButton* PlayButton;
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UButton* PauseButton;
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UButton* RestartButton;
	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UButton* EndButton;

	UPROPERTY(VisibleInstanceOnly, Category = "Focusview Panel Settings", meta = (BindWidget))
	class UButton* ExportVideoButton;

	void SwitchPanel(EFocusviewPanel NewPanel);
private:
	UEditAnimatedCameraOverviewButton* EditingButton;
	class UEditAnimatedCamFocusviewSubPanel* EdittingPanel;

	UFUNCTION()
	void OnPlayButtonClicked();
	UFUNCTION()
	void SimulatedTick();
	float CurrentDuration;
	float bShouldRescheduleTick;

	UFUNCTION()
	void OnPauseButtonClicked();

	UFUNCTION()
	void OnRestartButtonClicked();
	UFUNCTION()
	void OnEndButtonClicked();

	UFUNCTION()
	void HandleExportVideoButtonClicked();
};
