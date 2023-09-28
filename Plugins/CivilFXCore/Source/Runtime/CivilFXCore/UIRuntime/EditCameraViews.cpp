// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditCameraViews.h"
#include "Components/HorizontalBoxSlot.h"

void UEditCameraViews::NativeConstruct()
{
	Super::NativeConstruct();
	UHorizontalBoxSlot* LeftSlot = CastChecked<UHorizontalBoxSlot>( LeftBox->Slot);

}

void UEditCameraViews::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	GLog->Log("Syn");
}

void UEditCameraViews::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UHorizontalBoxSlot* MiddleSlot = CastChecked<UHorizontalBoxSlot>(MiddleBox->Slot);
	UHorizontalBoxSlot* RightSlot = CastChecked<UHorizontalBoxSlot>(RightBox->Slot);


	static float MinSize = 0.2;
	static float MaxSize = 0.7;

	static float Alpha = 0.0;
	Alpha += InDeltaTime;

	if (Alpha >= 1.0f)
	{
		Alpha = 0.0f;
	}

	float CurrentSize = FMath::LerpStable(0.2, 0.7, Alpha);

	FSlateChildSize Size;
	Size.Value = CurrentSize;
	MiddleSlot->SetSize(Size);

	Size.Value = MaxSize - CurrentSize;
	RightSlot->SetSize(Size);
}
