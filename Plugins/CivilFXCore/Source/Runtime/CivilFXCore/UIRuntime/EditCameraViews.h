// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditCameraViews.generated.h"


class UHorizontalBox;

UENUM()
enum class PanelSlotStatus : uint8
{
	Expanded,
	Normal,
	Shrunk
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class UEditCameraViews : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UHorizontalBox* Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidget* LeftBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidget* MiddleBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidget* RightBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MiddleBoxSize;

protected:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
