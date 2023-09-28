// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditOverviewButtonOptionsMenu.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UEditOverviewButtonOptionsMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	class UButton* GetDeleteButton();

protected:
	UPROPERTY(VisibleInstanceOnly, meta=(BindWidget))
	UButton* DeleteButton;
};
