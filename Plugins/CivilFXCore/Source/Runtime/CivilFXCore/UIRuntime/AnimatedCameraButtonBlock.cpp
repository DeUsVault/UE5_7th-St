// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "AnimatedCameraButtonBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"

void UAnimatedCameraButtonBlock::AddChild(UWidget* Content)
{
	ContentVerticalBox->AddChild(Content);
	UVerticalBoxSlot* ChildSlot = Cast<UVerticalBoxSlot>(Content->Slot);
	ChildSlot->SetPadding(FMargin(0, 5.0f, 0, 0));
}

void UAnimatedCameraButtonBlock::SetCategoryName(FString NewName)
{
	HeaderTextBlock->SetText(FText::FromString(NewName));
}
