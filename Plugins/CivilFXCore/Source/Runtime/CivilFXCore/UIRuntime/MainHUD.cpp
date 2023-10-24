/** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "MainHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AMainHUD::AMainHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> Widget_BP(TEXT("/CivilFXCore/UI_BPs/BP_MainHUDWidget"));
	MainHUDWidgetClass = Widget_BP.Class;
}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	check(MainHUDWidgetClass);
	MainHUDWidget = CreateWidget(GetWorld(), MainHUDWidgetClass);
	check(MainHUDWidget);
	MainHUDWidget->AddToViewport();
}