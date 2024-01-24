// /** Copyright (C) CivilFX 2023 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CivilFXCoreSettings.generated.h"

/**
 * 
 */
UCLASS(Config = CivilFXCoreConfig, DefaultConfig, BlueprintType)
class CIVILFXCORE_API UCivilFXCoreSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(config, EditAnywhere, Category = "API")
	bool bUseAPI = false;
	UPROPERTY(config, EditAnywhere, Category = "API", meta = (EditCondition = "bUseAPI"))
	FString EndPoint;
	UPROPERTY(config, EditAnywhere, Category = "API", meta = (EditCondition = "bUseAPI"))
	FString TestEndPoint;
	UPROPERTY(config, EditAnywhere, Category = "API", meta = (EditCondition = "bUseAPI"))
	FString ResourceToken;
};
