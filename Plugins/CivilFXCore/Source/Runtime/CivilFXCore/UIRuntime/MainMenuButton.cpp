// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "MainMenuButton.h"

FSlateColor UMainMenuButton::SelectedColor = FSlateColor(FLinearColor(0.313989, 0.520996, 0.068478, 1));
FSlateColor UMainMenuButton::DeselectedColor = FSlateColor(FLinearColor(1,1,1,1));

TWeakObjectPtr<UButton> UMainMenuButton::GetButton()
{
	return nullptr;
}

void UMainMenuButton::OnSelected()
{
	ICustomButtonInterface::OnSelected();
	UButton::WidgetStyle.Normal.TintColor = SelectedColor;
}

void UMainMenuButton::OnDeselected()
{
	ICustomButtonInterface::OnDeselected();
	UButton::WidgetStyle.Normal.TintColor = DeselectedColor;
}
