// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "TextableButton.h"

#include "Components/Button.h"

void UTextableButton::NativeConstruct()
{
	/*
	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, FName("OnSelected"));
	MainButton->OnClicked.Add(Delegate);
	*/
	MainButton->OnClicked.AddDynamic(this, &UTextableButton::OnSelected);
}

void UTextableButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MainText)
	{
		FSlateFontInfo FontInfo = MainText->Font;
		FontInfo.Size = TextSize;
		MainText->SetFont(FontInfo);

		MainText->SetText(ButtonText);
		MainText->SetJustification(TextJustification);
		//MainText->Font.Size = TextSize;


	}
}

void UTextableButton::SetButtonText(FText InText)
{
	ButtonText = InText;
	MainText->SetText(ButtonText);
}

//~ Begin ICustomButtonInterface
void UTextableButton::OnSelected()
{
	ICustomButtonInterface::OnSelected();
	MainButton->WidgetStyle.Normal.SetResourceObject(FilledTexture);
	GLog->Log("Selected is called");
}

void UTextableButton::OnDeselected()
{
	ICustomButtonInterface::OnDeselected();
	MainButton->WidgetStyle.Normal.SetResourceObject(BorderTexture);
	GLog->Log("Deselected");
}

TWeakObjectPtr<UButton> UTextableButton::GetButton()
{
	return MainButton;
}
//~ End ICustomButtonInterface