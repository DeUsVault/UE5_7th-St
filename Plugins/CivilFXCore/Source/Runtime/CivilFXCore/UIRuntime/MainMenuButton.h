// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "CustomButtonInterface.h"
#include "MainMenuButton.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UMainMenuButton : public UButton, public ICustomButtonInterface
{
	GENERATED_BODY()
	
public:
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	virtual TWeakObjectPtr<UButton> GetButton() override;
private:
	static FSlateColor SelectedColor;
	static FSlateColor DeselectedColor;

};
