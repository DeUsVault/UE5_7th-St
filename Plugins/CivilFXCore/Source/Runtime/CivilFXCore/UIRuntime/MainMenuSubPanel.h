// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuSubPanel.generated.h"


class UWidgetAnimation;
/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UMainMenuSubPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	const FText& GetPanelName() const;
	UWidgetAnimation* GetPanelAnimation() const;
	virtual void OnPanelSelected();
	virtual void OnPanelDeselected();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Panel Settings")
	FText PanelName;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, Category="Panel Settings", meta=(BindWidgetAnim))
	UWidgetAnimation* PanelAnim;
};
