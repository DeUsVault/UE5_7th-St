// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomButtonInterface.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "TextableButton.h"
#include "PhaseButton.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UPhaseButton : public UTextableButton
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
protected:

	UPROPERTY(EditAnywhere, Category = "Phase Button Settings")
	EPhaseType PhaseType;

	UPROPERTY(EditAnywhere, Category = "Phase Button Settings")
	EPhaseMode PhaseMode;

private:
	UFUNCTION()
	void OnPhaseButtonClicked();
};
