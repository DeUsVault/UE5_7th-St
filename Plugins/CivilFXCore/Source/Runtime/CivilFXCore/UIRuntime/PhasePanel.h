// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhasePanel.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UPhasePanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
protected:

	UPROPERTY(VisibleAnywhere, Category = "Phase Panel Settings", meta = (BindWidget))
	class UVerticalBox* Container;

	UPROPERTY(EditAnywhere, Category = "Phase Panel Settings")
	int32 DefaultIndex;

private:
	class UPhaseButton* CurrentSelectedButton;
	TArray<UPhaseButton*> PhaseButtons;

	UFUNCTION()
	void OnButtonClicked();
};
