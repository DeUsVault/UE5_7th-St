// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "MainMenuSubPanel.h"
#include "RenderingPanel.generated.h"

class UTextableButton;

UCLASS()
class CIVILFXCORE_API URenderingPanel : public UMainMenuSubPanel
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Video */
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    UTextableButton* Video4KButton;
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    UTextableButton* Video1080PUltraButton;
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    UTextableButton* Video1080PButton;
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    UTextableButton* StopRecordingButton;
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    class UVideoExportAdvSettingsPanel* VideoAdvSettingsPanel;
    UPROPERTY(VisibleInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    class UTextBlock* RecordedInformationText;

    UPROPERTY(BlueprintReadOnly)
    FString DataFolderDir;

    /** Image */
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    class UVerticalBox* ExportImageBox;
    UPROPERTY(EditInstanceOnly, Category="Rendering Panel Settings", meta=(BindWidget))
    UTextableButton* Image8KButton;
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    UTextableButton* Image4KButton;
    UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    UTextableButton* Image1080PButton;
    //UPROPERTY(EditInstanceOnly, Category = "Rendering Panel Settings", meta = (BindWidget))
    //UTextableButton* ImageWithUIButton;

private:

    UPROPERTY(Transient)
    class UCFXMediaOutput* VideoMediaOutput;
    UPROPERTY(Transient)
    class UCFXMediaCapture* VideoMediaCapture;
    UPROPERTY(Transient)
    class UTextureRenderTarget2D* VideoTextureRenderTarget;
    UPROPERTY(Transient)
    class ASceneCapture2D* SceneCapture2D;
    bool bCachedUseFixedFramerate;
    int32 CachedFixedFramerate;

    FTimerHandle UpdateInformationTimerHandle;

    UPROPERTY(Transient)
    class UFileMediaOutput* ScreenshotMediaOutput;
    UPROPERTY(Transient)
    class UFileMediaCapture* ScreenshotMediaCapture;
    UPROPERTY(Transient)
    class UTextureRenderTarget2D* ScreenShotTextureRenderTarget;

    /** Video functions */
    UFUNCTION()
    void HandleVideo4KButtonClicked();
    UFUNCTION()
    void HandleVideo1080PUltraButtonClicked();
    UFUNCTION()
    void HandleVideoCurrentButtonClicked();
    UFUNCTION()
    void HandleStopRecordingButtonClicked();
    void CaptureVideo(FIntPoint SourceSize, FIntPoint SampledSize, int32 BitRate, bool bWithUI=false);

    UFUNCTION()
    void UpdateRecordedInformation();

    UFUNCTION()
    void HandleAdvSettingsAreaExpansionChanged(class UExpandableArea* Area, bool bIsExpanded);

    /** Image functions */
    UFUNCTION()
    void HandleImage8KButtonClicked();
    UFUNCTION()
    void HandleImage4KButtonClicked();
    UFUNCTION()
    void HandleImage1080PButtonClicked();
    UFUNCTION()
    void OnImageWithUIButtonClicked();
    void CaptureScreenshot(int32 SizeX, int32 SizeY, bool bWithUI=false);
    UFUNCTION()
    void HandleCompleteScreenshot(bool bSuccess);

    void DeselectButton(UTextableButton* Button);

    FString CachedExploreFileDir;
};
