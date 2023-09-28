// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "EditAnimatedCamFocusviewSubPanel.h"
#include "EditAnimatedCamFocusviewMovement.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditAnimatedCamFocusviewMovement : public UEditAnimatedCamFocusviewSubPanel
{
	GENERATED_BODY()
	
public:
	
	void OnPanelVisible() override;
	void OnPanelHidden() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	class UEditAnimatedCameraPanel* PanelWithRenderWidgetViewport;


protected:
	UPROPERTY(EditInstanceOnly, Category="Focusview Movement Panel Settings")
	TSubclassOf<AActor> MovementObjectClass;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Movement Panel Settings")
	TSubclassOf<AActor> ArrowObjectClass;

	UPROPERTY(EditInstanceOnly, Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class USpinBox* ChevronSpeedSpinBox;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class UIntSpinBox* ChevronSizeSpinBox;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite ,Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class UButton* ColorButton;

	/** Some functionalities of MenuAnchor are handled via blueprints */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class UMenuAnchor* MenuAnchor;

	UPROPERTY(EditInstanceOnly, Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class USpinBox* HeightSpinBox;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class USpinBox* TiltAngleSpinBox;
	UPROPERTY(EditInstanceOnly, Category = "Focusview Movement Panel Settings", meta = (BindWidget))
	class UTextableButton* GenerateViewButton;
	

private:

	UFUNCTION()
	void OnGenerateViewButtonClicked();

	UFUNCTION()
	void OnLeftMouseDoubleCicked();
	UFUNCTION()
	void OnNotOverUILeftMouseDown();

	UFUNCTION()
	void HandleMenuAnchorOpenChanged(bool bIsOpen);
	UFUNCTION()
	void HandleChevronSpeedValueComitted(float NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void HandleChevronSizeValueCommitted(int32 NewValue, ETextCommit::Type CommitType);

	bool GetHitResultUnderCursor(FHitResult& OutHitResult);
	void CaptureComponent2D_DeProject(class USceneCaptureComponent2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection);
	void Capture2D_DeProject(class ASceneCapture2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection);
	
	class UWidgetSwitcher* ParentSwitcher;

	void UpdateVisualizer();
	void UpdateProjectedLines();
	int32 FindClosetPointInPoints(const FVector& Point, const TArray<FVector>& Points);

	UPROPERTY(Transient)
	TArray<AActor*> MovementObjects;
	UPROPERTY(Transient)
	TArray<AActor*> ArrowObjects;

	UPROPERTY(Transient)
	class AChevronCreator* Chevron;

	TArray<FVector> TempLocations;
	TArray<FRotator> TempRotations;
	FTimerHandle TimerHandle;
	bool bAddNewObject;
};
