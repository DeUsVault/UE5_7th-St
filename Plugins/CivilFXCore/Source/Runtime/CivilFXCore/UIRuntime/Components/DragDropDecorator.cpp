// Fill out your copyright notice in the Description page of Project Settings.


#include "DragDropDecorator.h"

#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/STextBlock.h"

#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "UMG"

UDragDropDecorator::UDragDropDecorator()
{
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	}
}

void UDragDropDecorator::SetBorderBrushColor(const FLinearColor& InBrushColor)
{
	BorderBrushColor = InBrushColor;
	if (MyDecorator.IsValid())
	{
		MyDecorator->SetBorderBackgroundColor(InBrushColor);
	}
}

void UDragDropDecorator::SetText(const FText& InText)
{
	Text = InText;
	if (MyTextBlock.IsValid())
	{
		MyTextBlock->SetText(Text);
	}
}

void UDragDropDecorator::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyDecorator->SetBorderImage(&Background);
	MyDecorator->SetBorderBackgroundColor(BorderBrushColor);

	MyTextBlock->SetText(Text);
	MyTextBlock->SetFont(Font);
}

void UDragDropDecorator::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyDecorator.Reset();
	MyTextBlock.Reset();
}

#if WITH_EDITOR
const FText UDragDropDecorator::GetPaletteCategory()
{
	return LOCTEXT("CivilFX Custom", "CivilFX Custom");
}
#endif

TSharedRef<SWidget> UDragDropDecorator::RebuildWidget()
{
	SAssignNew(MyDecorator, SBorder)
	.BorderBackgroundColor(BorderBrushColor)
	[
		SNew(SScaleBox)
		[
			SAssignNew(MyTextBlock, STextBlock)
			.Text(Text)
			.SimpleTextMode(true)
			.Font(Font)
		]
	];

	return MyDecorator.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE