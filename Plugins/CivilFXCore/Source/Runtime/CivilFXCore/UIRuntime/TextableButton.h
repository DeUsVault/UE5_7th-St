// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CustomButtonInterface.h"
#include "TextableButton.generated.h"

class UButton;

UCLASS()
class CIVILFXCORE_API UTextableButton : public UUserWidget, public ICustomButtonInterface
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	//~ Begin ICustomButtonInterface
	void OnSelected() override;
	void OnDeselected() override;
	TWeakObjectPtr<UButton> GetButton() override;
	//~ End ICustomButtonInterface

	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText);

protected:
	UPROPERTY(VisibleAnywhere, Category="Textable Button Settings", meta=(BindWidget))
	UButton* MainButton;

	UPROPERTY(VisibleAnywhere, Category = "Textable Button Settings", meta = (BindWidget))
	UTextBlock* MainText;

	UPROPERTY(EditAnywhere, Category = "Textable Button Settings")
	FText ButtonText;
	UPROPERTY(EditAnywhere, Category = "Textable Button Settings")
	TEnumAsByte< ETextJustify::Type> TextJustification;
	UPROPERTY(EditAnywhere, Category = "Textable Button Settings")
	int32 TextSize = 12;

	UPROPERTY(EditAnywhere, Category = "Textable Button Settings")
	UObject* BorderTexture;
	UPROPERTY(EditAnywhere, Category = "Textable Button Settings")
	UObject* FilledTexture;
};
