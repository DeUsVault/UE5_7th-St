// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "MainMenuSubPanel.h"

const FText& UMainMenuSubPanel::GetPanelName() const
{
	return PanelName;
}

UWidgetAnimation* UMainMenuSubPanel::GetPanelAnimation() const
{
	return PanelAnim;
}

void UMainMenuSubPanel::OnPanelSelected()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(PanelAnim, 0, 1, EUMGSequencePlayMode::Reverse);
}

void UMainMenuSubPanel::OnPanelDeselected()
{
	SetVisibility(ESlateVisibility::Hidden);
}