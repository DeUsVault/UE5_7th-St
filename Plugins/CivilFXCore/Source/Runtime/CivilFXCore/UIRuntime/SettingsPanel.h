// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "MainMenuSubPanel.h"
#include "SunPosition.h" //needed otherwise SunPosition will break
#include "SettingsPanel.generated.h"
/**
 * Note: Logics for Sun position is implemented in blueprint graph.
 */
UCLASS()
class CIVILFXCORE_API USettingsPanel : public UMainMenuSubPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
private:
	FSunPositionData SunPositionData; //needed otherwise SunPosition will break
};
