// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "ImageButton.h"
#include "EditAnimatedCameraManualButton.generated.h"

class UButton;

UCLASS()
class CIVILFXCORE_API UEditAnimatedCameraManualButton : public UImageButton
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnParentContainerRequested, int32);
	FOnParentContainerRequested OnRequestAddChild;
	FOnParentContainerRequested OnRequestRemoveChild;
	FOnParentContainerRequested OnRequestRefreshChild;

	void OnCreated(const FVector& Location, const FRotator& Rotation);
	void GetView(FVector& OutLocation, FRotator& OutRotation);
	void SetView(const FVector& InLocation, const FRotator& InRotation);

	void UpdateCurrentIndex(int32 InIndex, int32 ChildrenNum);
	int32 GetCurrentIndex() const;

	virtual void NativeConstruct() override;
protected:
	UPROPERTY(EditInstanceOnly, Category = "Edit Manual Button Settings", meta = (BindWidget))
	UButton* DeleteButton;
	UPROPERTY(EditInstanceOnly, Category = "Edit Manual Button Settings", meta = (BindWidget))
	UButton* RefreshButton;
	UPROPERTY(EditInstanceOnly, Category = "Edit Manual Button Settings", meta = (BindWidget))
	UButton* AddLeftButton;
	UPROPERTY(EditInstanceOnly, Category = "Edit Manual Button Settings", meta = (BindWidget))
	UButton* AddRightButton;
	UPROPERTY(EditInstanceOnly, Category = "Edit Manual Button Settings", meta = (BindWidget))
	class UTextBlock* DebugTextBlock;
private:
	FVector Location;
	FRotator Rotation;
	int32 CurrentIndex;
	void SetAddLeftButtonVisibility(ESlateVisibility Visibility);
	UFUNCTION()
	void OnMainButtonClicked();
	UFUNCTION()
	void OnDeleteButtonClicked();
	UFUNCTION()
	void OnRefreshButtonClicked();
	UFUNCTION()
	void OnAddLeftButtonClicked();
	UFUNCTION()
	void OnAddRightButtonClicked();
};
