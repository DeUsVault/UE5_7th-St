/** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMainHUD();
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MainHUDWidget;

protected:
	UPROPERTY()
	TSubclassOf<class UUserWidget> MainHUDWidgetClass;
};
