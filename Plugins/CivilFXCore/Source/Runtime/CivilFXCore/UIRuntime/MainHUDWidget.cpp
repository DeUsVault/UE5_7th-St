/** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "MainHUDWidget.h"
#include "Components/Button.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "MainMenu.h"
#include "Interfaces/IPluginManager.h"
#include "CivilFXCore/CommonCore/CivilFXCoreSettings.h"

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(HambergerButton);

	/*
	FScriptDelegate OnClickedDelegate;
	OnClickedDelegate.BindUFunction(this, FName("OnHambergerButtonClicked"));
	HambergerButton->OnClicked.Add(OnClickedDelegate);
	*/
	HambergerButton->OnClicked.AddDynamic(this, &UMainHUDWidget::OnHambergerButtonClicked);

	if (MainMenu)
	{
		MainMenu->SetReferenceToHamburgerButton(HambergerButton);
	}

	const UCivilFXCoreSettings* CoreSettings = GetDefault<UCivilFXCoreSettings>();
	if (CoreSettings->bUseAPI)
	{
		ExitButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

//
//bool UMainHUDWidget::IsPixelStreamingEnabled() const
//{
//	TSharedPtr<IPlugin> PixelStreamPlugin = IPluginManager::Get().FindPlugin(TEXT("PixelStreaming"));
//	return PixelStreamPlugin.IsValid();
//}


void UMainHUDWidget::OnHambergerButtonClicked()
{
	Status = Status == EPanelStatus::Opened ? EPanelStatus::Closed : EPanelStatus::Opened;

	if (Status == EPanelStatus::Opened)
	{
		//open the panel
		SetButtonTextures(HambergerOpenedTexture);
		PlayAnimation(MainMenuPanelAnim, 0.f, 1, EUMGSequencePlayMode::Reverse);
	}
	else
	{
		//close the panel
		SetButtonTextures(HambergerClosedTexture);
		PlayAnimation(MainMenuPanelAnim, 0.f, 1, EUMGSequencePlayMode::Forward);
	}

	//disable the button for awhile
	SetButtonInteractable(false);
	const float DelayTime = 1.0f; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UMainHUDWidget::SetButtonInteractable, true), DelayTime, false);
}

void UMainHUDWidget::SetButtonTextures(UObject* NewTexture)
{
	HambergerButton->WidgetStyle.Normal.SetResourceObject(NewTexture);
	HambergerButton->WidgetStyle.Hovered.SetResourceObject(NewTexture);
	HambergerButton->WidgetStyle.Pressed.SetResourceObject(NewTexture);
}

void UMainHUDWidget::SetButtonInteractable(bool bInteractable)
{
	HambergerButton->SetIsEnabled(bInteractable);
}
