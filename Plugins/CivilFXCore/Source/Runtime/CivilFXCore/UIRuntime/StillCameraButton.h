// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StillCameraButton.generated.h"


UCLASS()
class CIVILFXCORE_API UStillCameraButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetCameraLocationAndRotation(const FVector& InLocation, const FRotator& InRotation);
	void SetCameraButtonName(FText InCameraName);

	FString GetSerializedString(int32 Indent) const;

	FVector Location;
	FRotator Rotation;
	FText CameraName;

	virtual void NativeConstruct() override;
	class UTextableButton* GetMainButton() const
	{
		return MainButton;
	}
protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Still Camera Button Settings", meta=(BindWidget))
	class UTextableButton* MainButton;

	UPROPERTY(VisibleInstanceOnly, Category = "Still Camera Button Settings", meta=(BindWidget))
	class UButton* DeleteButton;

private:

	bool bHasTransformData;


	UFUNCTION()
	void OnMainButtonClicked();

	UFUNCTION()
	void OnDeleteButtonClicked();

	class ACivilFXPawn* PlayerPawn;
};
