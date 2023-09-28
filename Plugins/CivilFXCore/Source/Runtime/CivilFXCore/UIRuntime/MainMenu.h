/** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UWidgetAnimation;
class UMainMenuButton;
class UMainMenuSubPanel;
class UTextBlock;

UENUM()
enum class EPanel : uint8
{
	Navigation = 0,
	Rendering,
	Settings,
	TrafficSettings,
	About
};

/**
 * 
 */
UCLASS(Abstract)
class CIVILFXCORE_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Main Menu Panel Settings", meta=(BindWidget, EditCondition="1==2"))
	UMainMenuButton* NavButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta=(BindWidget))
	UMainMenuSubPanel* NavPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget, EditCondition = "1==2"))
	UMainMenuButton* RendButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget))
	UMainMenuSubPanel* RendPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget, EditCondition = "1==2"))
	UMainMenuButton* SettButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget))
	UMainMenuSubPanel* SettPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget, EditCondition = "1==2"))
	UMainMenuButton* TraffSettButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta=(BindWidget))
	UMainMenuSubPanel* TraffSettPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget, EditCondition = "1==2"))
	UMainMenuButton* AbtButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget))
	UMainMenuSubPanel* AbtPanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Main Menu Panel Settings", meta = (BindWidget))
	UTextBlock* PanelNameText;

	UPROPERTY(EditAnywhere, Category = "Main Menu Panel Settings")
	EPanel DefaultPanel;


	void SetReferenceToHamburgerButton(class UButton *ButtonRef) const;

protected:
	virtual void NativeConstruct() override;
	

private:
	UPROPERTY()
	UMainMenuSubPanel* SelectedPanel;
	UPROPERTY()
	UMainMenuButton* SelectedButton;

	UFUNCTION()
	void OnPanelSelected(UMainMenuSubPanel* NewPanel, UMainMenuButton* NewButton);
	UFUNCTION()
	void OnNavigationPanelSelected();
	UFUNCTION()
	void OnRenderingPanelSelected();
	UFUNCTION()
	void OnSettingsPanelSelected();
	UFUNCTION()
	void OnTrafficSettingsSelected();
	UFUNCTION()
	void OnAboutPanelSelected();
};
