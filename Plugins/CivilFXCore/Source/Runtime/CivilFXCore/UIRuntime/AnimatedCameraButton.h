// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimatedCameraNodeData.h"
#include "AnimatedCameraButton.generated.h"

UCLASS()
class CIVILFXCORE_API UAnimatedCameraButton : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetCameraNodeData(const FAnimatedCameraNodeData& InCameraData);
	void SetCameraButtonName(FText InCameraName);
	void StopChevron();

	class UTextableButton* GetMainButton() const
	{
		return MainButton;
	}
protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Still Camera Button Settings", meta = (BindWidget))
	class UTextableButton* MainButton;

	UPROPERTY(VisibleInstanceOnly, Category = "Still Camera Button Settings", meta = (BindWidget))
	class UButton* PreviewButton;

private:
	FAnimatedCameraNodeData CameraNodeData;

	UFUNCTION()
	void OnMainButtonClicked();

	UFUNCTION()
	void OnPreviewButtonClicked();

	class ACivilFXPawn* PlayerPawn;

	UPROPERTY(Transient)
	class AChevronCreator* ChevronActor;

};
