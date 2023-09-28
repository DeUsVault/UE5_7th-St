/** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CustomButtonInterface.h"

ICustomButtonInterface::ICustomButtonInterface() :
	ButtonState(EButtonSelectionState::Deselected),
	bAllowDeselection(false)
{

}

void ICustomButtonInterface::OnSelected()
{
	if (bAllowDeselection && ButtonState == EButtonSelectionState::Selected)
	{
		OnDeselected();
	}
	else
	{
		ButtonState = EButtonSelectionState::Selected;
	}
}

void ICustomButtonInterface::OnDeselected()
{
	ButtonState = EButtonSelectionState::Deselected;
}

bool ICustomButtonInterface::IsButtonSelected() const
{
	return ButtonState == EButtonSelectionState::Selected;
}

void ICustomButtonInterface::SetAllowDeselection(bool bAllowed)
{
	bAllowDeselection = bAllowed;
}
