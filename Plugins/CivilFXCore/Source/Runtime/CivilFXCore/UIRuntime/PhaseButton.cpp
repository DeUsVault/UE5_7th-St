// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "PhaseButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UPhaseButton::NativeConstruct()
{
	Super::NativeConstruct();

	/*
	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, FName("OnPhaseButtonClicked"));
	MainButton->OnClicked.Add(Delegate);
	*/
	MainButton->OnClicked.AddDynamic(this, &UPhaseButton::OnPhaseButtonClicked);
}

void UPhaseButton::OnPhaseButtonClicked()
{
	UPhaseManager* Manager = GetGameInstance()->GetSubsystem<UPhaseManager>();
	check(Manager);
	Manager->SwitchPhase(PhaseType, PhaseMode);
}