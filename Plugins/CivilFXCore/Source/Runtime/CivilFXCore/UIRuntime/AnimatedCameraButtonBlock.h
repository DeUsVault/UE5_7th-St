// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimatedCameraButtonBlock.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UAnimatedCameraButtonBlock : public UUserWidget
{
	GENERATED_BODY()
public:
	void AddChild(UWidget* Content);
	void SetCategoryName(FString NewName);

protected:
	UPROPERTY(EditInstanceOnly, Category = "Animated Button Block Settings", meta = (BindWidget))
	class UTextBlock* HeaderTextBlock;
	UPROPERTY(EditInstanceOnly, Category = "Animated Button Block Settings", meta = (BindWidget))
	class UVerticalBox* ContentVerticalBox;
};
