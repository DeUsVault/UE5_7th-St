// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "MainMenuSubPanel.h"
#include "StillCameraButton.h"
#include "EditAnimatedCameraPanel.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "AnimatedCameraNodeData.h"
#include "AnimatedCameraButton.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "Navigation/StillCameraViewItem.h"
#include "NavigationPanel.generated.h"

class UButton;
class UScrollBox;
class UAnimatedCameraView;
class UStillCameraView;
class ACivilFXPawn;

USTRUCT()
struct FNavigationCameraData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAnimatedCameraNodeData> AnimatedCamera;
	UPROPERTY()
	TArray<FCameraViewInfo> StillCameras;
};

UCLASS()
class CIVILFXCORE_API UNavigationPanel : public UMainMenuSubPanel
{
	GENERATED_BODY()

public:

	/**
	*	Reference to the HUD hamburger button
	*	This reference is propagate down to here from the HUD via SetReferenceToHamburgerButton()
	*/
	UButton* HamburgerButton;
	void SetReferenceToHamburgerButton(UButton* HamburgerButtonRef);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/*Animated Cameras*/
	UPROPERTY(EditInstanceOnly, Category = "Navigation Panel Settings", meta = (BindWidget))
	UButton* EditAnimatedCameraButton;
	UPROPERTY(EditInstanceOnly, Category = "Navigation Panel Settings", meta = (BindWidget))
	UAnimatedCameraView* AnimatedCameraView;
	UPROPERTY(EditInstanceOnly, Category = "Navigation Panel Settings")
	TSubclassOf<class UAnimatedCameraButtonBlock> AnimatedCameraButtonBlockClass;

	/*Still Cameras*/
	UPROPERTY(EditInstanceOnly, Category="Navigation Panel Settings", meta=(BindWidget))
	UButton* AddStillCameraButton;
	UPROPERTY(EditInstanceOnly, Category = "Navigation Panel Settings", BlueprintReadOnly, meta = (BindWidget))
	UStillCameraView* StillCameraView;

private:
	FString DataFolderDir;
	FString CameraConfigFileDir;

	/*Animated Cameras*/
	UPROPERTY(Transient)
	TSubclassOf<UEditAnimatedCameraPanel> EditAnimatedCameraClass;
	TArray<FAnimatedCameraNodeData> CameraNodeDatas;
	void RefreshAnimatedCameraContainer();

	UFUNCTION()
	void HandleEditAnimatedCameraButtonClicked();
	UFUNCTION()
	void HandleEditAnimatedCameraPanelExited(TArray<FAnimatedCameraNodeData>& InOutCameraNodeDatas);
	UFUNCTION()
	void HandleNewPhaseSwitch(EPhaseType PhaseType, EPhaseMode PhaseMode);
	//~

	/** Still Cameras */
	UFUNCTION()
	void HandleAddStillCameraButtonClicked();
	UFUNCTION()
	void AddNewStillCameraToScrollBoxDelegate(FText CameraName);
	void AddNewStillCameraToScrollBox(const FText& CameraName, const FText& CameraCategory, const FRotator& Rotation, const FVector& Location);
	FText HandleStillComboBoxCategoryGetText();
	void HandleStillComboBoxCategoryTextCommitted(const FText& InText);
	FText CurrentEditingStillCameraCategoryText;
	//~

	ACivilFXPawn* PlayerPawn;
};
