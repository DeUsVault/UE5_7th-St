/** Copyright (C) CivilFX 2020 - All Rights Reserved */


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UButton;
class UWidgetAnimation;
class UTexture2D;
class UUserWidget;
class UMainMenu;
class UCivilFXCoreSettings;
class UButton;

UENUM()
enum class EPanelStatus
{
    Opened,
    Closed
};
/**
 * 
 */
UCLASS(ABSTRACT)
class CIVILFXCORE_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category="Main Widget Settings", meta=(BindWidget, EditCondition="1==2")) /*EditCondition="1==2" to force non-editable since VisibleAnywhere does not seem to work on UButton*/
    UButton* HambergerButton;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, Category = "Main Widget Settings", meta=(BindWidgetAnim))
    UWidgetAnimation* MainMenuPanelAnim;

    UPROPERTY(EditAnywhere, Category = "Main Widget Settings")
    UObject* HambergerOpenedTexture;

    UPROPERTY(EditAnywhere, Category = "Main Widget Settings")
    UObject* HambergerClosedTexture;

    EPanelStatus Status = EPanelStatus::Opened;

     /*Additional references*/
    UPROPERTY(meta=(BindWidget))
    UMainMenu* MainMenu;

   UPROPERTY(meta=(BindWidget))
   UButton* ExitButton;

protected:
    UFUNCTION()
    void OnHambergerButtonClicked();

    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    bool IsPixelStreamingEnabled() const;

private:
    /**
    * Set new texture for button
    * The color for each state is set in the blueprint
    */
    void SetButtonTextures(UObject* NewTexture);

    /** Timer stuffs */
    FTimerHandle TimerHandle;
    void SetButtonInteractable(bool bInteractable);
};
