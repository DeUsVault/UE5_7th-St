// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "ImageButton.h"
#include "AnimatedCameraNodeData.h"
#include "EditAnimatedCameraOverviewButton.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditAnimatedCameraOverviewButton : public UImageButton
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void SetCameraName(const FText& NewName);
    FText GetCameraName() const;
    void SetHighlightText(FText InText);
    void UpdateCameraNodeData();
    void SetCameraNodeData(TSharedPtr<FAnimatedCameraNodeData> InCameraData, int32 InId);
    TSharedPtr<FAnimatedCameraNodeData> GetCameraNodeData() const;
    int32 GetCameraNodeId() const;

    bool bPendingDelete;

    class UEditAnimatedCameraOverviewPanel* ParentPanel;


protected:
    UPROPERTY(VisibleInstanceOnly, Category="Overview Button Settings", meta=(BindWidget))
    class UTextBlock* CameraNameTextBox;

    UPROPERTY(VisibleInstanceOnly, Category = "Overview Button Settings", meta = (BindWidget))
    class UButton* MoreOptionsButton;
    UPROPERTY(VisibleInstanceOnly, Category = "Overview Button Settings", meta = (BindWidget))
    class UMenuAnchor* MoreOptionsMenuAnchor;

private:
    TSharedPtr<FAnimatedCameraNodeData> CameraNodeData;
    int32 Id = -1;
  
    UPROPERTY(Transient)
    UWidget* AnchorMenuContent;

    UFUNCTION()
    void HandleDeleteButtonClicked();

    UFUNCTION()
    void HandleConfirmedDelete(FText Unused);
    UFUNCTION()
    void HandleCanceledDelete();

    UFUNCTION()
    void HandleMoreOptionsButtonClicked();
    UFUNCTION()
    UWidget* HandleMenuAnchorGetContent();
};
