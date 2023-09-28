// Fill out your copyright notice in the Description page of Project Settings.


#include "CivilFXGameMode.h"
#include "CivilFXPawn.h"
#include "Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "CivilFXCore/UIRuntime/MainHUD.h"

ACivilFXGameMode::ACivilFXGameMode() :
	Super()
{
	//setup hud
	HUDClass = AMainHUD::StaticClass();

	//setup pawn
	DefaultPawnClass = ACivilFXPawn::StaticClass();
}

// void ACivilFXGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
// {
// 	Super::InitGame(MapName, Options, ErrorMessage);
// 
// 	
// 	//set up resolution
// 	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
// 	float DefaultWidth = 1280;
// 	float DefaultHeight = 720;
// 
// 	UserSettings->RequestResolutionChange(DefaultWidth, DefaultHeight, EWindowMode::Windowed, false);
// 	auto Res = UserSettings->GetScreenResolution();
// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Res.ToString());
// 
// 	
// }
