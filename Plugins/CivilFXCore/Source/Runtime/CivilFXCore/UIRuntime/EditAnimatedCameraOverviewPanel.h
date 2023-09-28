// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditAnimatedCameraOverviewButton.h"
#include "AnimatedCameraNodeData.h"
#include "EditAnimatedCameraOverviewPanel.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditAnimatedCameraOverviewPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void AddChild(bool bChildSelected=false);
	void AddChild(TSharedPtr<FAnimatedCameraNodeData> CameraData, bool bChildSelected=false);
	void DisplaySearchedChildren(const FText& SeachedText);
	void SetPropertyPanel(class UEditAnimatedCameraPropertyPanel* InPropertyPanel);
	TArray<FAnimatedCameraNodeData> GetAllCameraNodeData();

	void DeleteChild(UEditAnimatedCameraOverviewButton* DeletingButton);

protected:
	UPROPERTY(EditInstanceOnly, Category="Overview Panel Settings", meta=(BindWidget))
	class UScrollBox* Container;
	UPROPERTY(EditInstanceOnly, Category = "Overview Panel Settings")
	TSubclassOf<UEditAnimatedCameraOverviewButton> ImageButtonClass;

private:
	UEditAnimatedCameraOverviewButton* CurrentSelectedButton;
	UFUNCTION()
	void OnEditAnimatedCameraButtonSelected();
	class UEditAnimatedCameraPropertyPanel* PropertyPanel;
};
