/** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "MainMenu.h"
#include "MainMenuButton.h"
#include "MainMenuSubPanel.h"
#include "Components/TextBlock.h"
#include "NavigationPanel.h"


void UMainMenu::SetReferenceToHamburgerButton(class UButton* ButtonRef) const
{
	CastChecked<UNavigationPanel>(NavPanel)->SetReferenceToHamburgerButton(ButtonRef);
}

void UMainMenu::NativeConstruct()
{
	GLog->Log("Native Construct");

	Super::NativeConstruct();

	/*
	FScriptDelegate NavDelegate;
	NavDelegate.BindUFunction(this, FName("OnNavigationPanelSelected"));
	NavButton->OnClicked.Add(NavDelegate);
	*/
	NavButton->OnClicked.AddDynamic(this, &UMainMenu::OnNavigationPanelSelected);

	/*
	FScriptDelegate RendDelegate;
	RendDelegate.BindUFunction(this, FName("OnRenderingPanelSelected"));
	RendButton->OnClicked.Add(RendDelegate);
	*/
	RendButton->OnClicked.AddDynamic(this, &UMainMenu::OnRenderingPanelSelected);

	/*
	FScriptDelegate SettDelegate;
	SettDelegate.BindUFunction(this, FName("OnSettingsPanelSelected"));
	SettButton->OnClicked.Add(SettDelegate);
	*/
	SettButton->OnClicked.AddDynamic(this, &UMainMenu::OnSettingsPanelSelected);

	/*
	FScriptDelegate TraffSettDelegate;
	TraffSettDelegate.BindUFunction(this, FName("OnTrafficSettingsSelected"));
	TraffSettButton->OnClicked.Add(TraffSettDelegate);
	*/
	TraffSettButton->OnClicked.AddDynamic(this, &UMainMenu::OnTrafficSettingsSelected);

	/*
	FScriptDelegate AbtDelegate;
	AbtDelegate.BindUFunction(this, FName("OnAboutPanelSelected"));
	AbtButton->OnClicked.Add(AbtDelegate);
	*/
	AbtButton->OnClicked.AddDynamic(this, &UMainMenu::OnAboutPanelSelected);

	//deselect all buttons and panels
	NavButton->OnDeselected();
	RendButton->OnDeselected();
	SettButton->OnDeselected();
	TraffSettButton->OnDeselected();
	AbtButton->OnDeselected();

	NavPanel->OnPanelDeselected();
	RendPanel->OnPanelDeselected();
	SettPanel->OnPanelDeselected();
	TraffSettPanel->OnPanelDeselected();
	AbtPanel->OnPanelDeselected();

	UMainMenuButton* NewlySelectedButton = nullptr;
	switch (DefaultPanel)
	{
	case EPanel::Navigation:
		NewlySelectedButton = NavButton;
		break;
	case EPanel::Rendering:
		NewlySelectedButton = RendButton;
		break;
	case EPanel::Settings:
		NewlySelectedButton = SettButton;
		break;
	case EPanel::TrafficSettings:
		NewlySelectedButton = TraffSettButton;
		break;
	case EPanel::About:
		NewlySelectedButton = AbtButton;
		break;
	}

	NewlySelectedButton->OnClicked.Broadcast();
}

void UMainMenu::OnPanelSelected(UMainMenuSubPanel* NewPanel, UMainMenuButton* NewButton)
{
	if (SelectedButton)
	{
		SelectedButton->OnDeselected();
	}

	if (SelectedPanel)
	{
		SelectedPanel->OnPanelDeselected();
	}

	PanelNameText->SetText(NewPanel->GetPanelName());

	NewButton->OnSelected();
	NewPanel->OnPanelSelected();

	SelectedPanel = NewPanel;
	SelectedButton = NewButton;
}

void UMainMenu::OnNavigationPanelSelected()
{
	OnPanelSelected(NavPanel, NavButton);
}

void UMainMenu::OnRenderingPanelSelected()
{
	OnPanelSelected(RendPanel, RendButton);
}

void UMainMenu::OnSettingsPanelSelected()
{
	OnPanelSelected(SettPanel, SettButton);
}

void UMainMenu::OnTrafficSettingsSelected()
{
	OnPanelSelected(TraffSettPanel, TraffSettButton);
}

void UMainMenu::OnAboutPanelSelected()
{
	OnPanelSelected(AbtPanel, AbtButton);
}

