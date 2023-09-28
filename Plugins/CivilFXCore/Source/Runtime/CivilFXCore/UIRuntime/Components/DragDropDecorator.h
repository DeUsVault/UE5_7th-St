// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "DragDropDecorator.generated.h"

class SBorder;
class STextBlock;

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UDragDropDecorator : public UContentWidget
{
	GENERATED_BODY()

public:
	UDragDropDecorator();

	UPROPERTY(EditAnywhere)
	FText Text;

	UPROPERTY(EditAnywhere)
	FLinearColor BorderBrushColor;

	/** Brush to drag as the background */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (DisplayName = "Brush"))
	FSlateBrush Background;

	/** The font to render the text with */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateFontInfo Font;

	void SetBorderBrushColor(const FLinearColor& InBrushColor);
	void SetText(const FText& InText);

	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

protected:
	TSharedPtr<SBorder> MyDecorator;
	TSharedPtr<STextBlock> MyTextBlock;
};
