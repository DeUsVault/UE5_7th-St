// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "PhasePanel.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PhaseButton.h"

void UPhasePanel::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (Container)
	{
		PhaseButtons.Empty();
		TArray<UWidget*> Children = Container->GetAllChildren();
		for (UWidget* Child : Children)
		{
			UPhaseButton* Button = Cast<UPhaseButton>(Child);
			if (Button)
			{
				PhaseButtons.Add(Button);
			}
		}
	}
	DefaultIndex = FMath::Clamp(DefaultIndex, 0, PhaseButtons.Num() - 1);
}
void UPhasePanel::NativeConstruct()
{
	Super::NativeConstruct();

	/**/
	// Set to LRT on start-up
	CurrentSelectedButton = PhaseButtons[2];


	//invoke the default button
	//Do this first so this button does not get deselected on next tick
	//before of OnButtonClicked()
	CurrentSelectedButton->GetButton()->OnClicked.Broadcast();

	for (UPhaseButton* Button : PhaseButtons)
	{
		Button->GetButton()->OnClicked.AddDynamic(this, &UPhasePanel::OnButtonClicked);
	}
}



void UPhasePanel::OnButtonClicked()
{
	//Execute on next tick due to how functions are bind to OnCliked()
	//They are executed in reversed order (last in first executed)
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		UPhaseButton* NewSelectedButton = nullptr;
		for (UPhaseButton* Button : PhaseButtons)
		{
			if (Button->IsButtonSelected() && Button != CurrentSelectedButton)
			{
				NewSelectedButton = Button;
				break;
			}
		}
		if (NewSelectedButton)
		{
			CurrentSelectedButton->OnDeselected();
			CurrentSelectedButton = NewSelectedButton;
		}
	});
}