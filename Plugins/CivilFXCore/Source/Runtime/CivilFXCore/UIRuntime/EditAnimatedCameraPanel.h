// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "MainMenuSubPanel.h"
#include "AnimatedCameraNodeData.h"
#include "EditAnimatedCameraPanel.generated.h"

class UEditAnimatedCameraOverviewButton;
class UNavigationPanel;
struct FCameraNodeData_ID;

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditAnimatedCameraPanel : public UMainMenuSubPanel
{
	GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEditAnimatedCameraPanelExited, TArray<FAnimatedCameraNodeData>&, const TArray<FCameraNodeData_ID>&)
    FOnEditAnimatedCameraPanelExited OnEditAnimatedCameraPanelExited;

    virtual void NativeConstruct() override;
    virtual void OnPanelSelected() override;
    void RefreshPanel(const TArray<FAnimatedCameraNodeData>& InCameraNodeDatas,
        const TArray<FCameraNodeData_ID>& InIds);

    class ASceneCapture2D* GetSceneCapture2D () const { return ScenceCapture2D; };

    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UEditAnimatedCameraOverviewPanel* OverviewPanel;

    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UEditAnimatedCameraPropertyPanel* PropertyPanel;

    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UEditAnimatedCameraFocusviewPanel* FocusviewPanel;

    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UImage* RenderTextureViewport;

    void OnBeginRecording();
    void OnEndRecording();

protected:

    UPROPERTY(EditInstanceOnly, Category="Edit Animated Camera Settings", meta=(BindWidget))
    class UOverlay* Container;
    
    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UEditableTextBox* SearchCameraInputBox;

    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UButton* AddNodeButton;
    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings", meta = (BindWidget))
    class UButton* ExitButton;

    UPROPERTY(EditInstanceOnly, Category = "Edit Animated Camera Settings")
    class UTextureRenderTarget2D* TargetTexture;

    /**  */


    UPROPERTY(Transient)
    class ASceneCapture2D* ScenceCapture2D;
    class ACivilFXPawn* PlayerPawn;

private:
    UFUNCTION()
    void HandleAddNodeButtonClicked();

    UFUNCTION()
    void HandleExitButtonClicked();

    UFUNCTION()
    void HandleSearchBoxTextChanged(const FText& InputText);

};
