/** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/Button.h"
#include "CustomButtonInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UCustomButtonInterface : public UInterface
{
	GENERATED_BODY()
};


UENUM()
enum class EButtonSelectionState : uint8
{
	Selected = 0,
	Deselected
};

class ICustomButtonInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	ICustomButtonInterface();

	UFUNCTION()
	virtual void OnSelected();
	UFUNCTION()
	virtual void OnDeselected();

	bool IsButtonSelected() const;

	/**
	 *	If deselection is allowed, the button will return to deselected state 
		when OnSelected() is called while the button is currently selected.
		It maybe is neccessary to schedule your override OnDeselect() on the next frame. 
	 */
	void SetAllowDeselection(bool bAllowed);

	virtual TWeakObjectPtr<UButton> GetButton() = 0;

protected:
	EButtonSelectionState ButtonState;

private:
	bool bAllowDeselection;
};
