// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CivilFXGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API ACivilFXGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
    ACivilFXGameMode();
    //virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
