// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IntSpinBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "UMG"

//https://forums.unrealengine.com/development-discussion/content-creation/1641116-creating-an-integer-spin-box-for-widget
/////////////////////////////////////////////////////
// UIntSpinBox

UIntSpinBox::UIntSpinBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		Font = FSlateFontInfo(RobotoFontObj.Object, 12, FName("Bold"));
	}

	// Grab other defaults from slate arguments.
	SSpinBox<int32>::FArguments Defaults;

	Value = Defaults._Value.Get();
	MinValue = Defaults._MinValue.Get().Get(0.0f);
	MaxValue = Defaults._MaxValue.Get().Get(0.0f);
	MinSliderValue = Defaults._MinSliderValue.Get().Get(0.0f);
	MaxSliderValue = Defaults._MaxSliderValue.Get().Get(0.0f);
	Delta = Defaults._Delta.Get();
	SliderExponent = Defaults._SliderExponent.Get();
	MinDesiredWidth = Defaults._MinDesiredWidth.Get();
	ClearKeyboardFocusOnCommit = Defaults._ClearKeyboardFocusOnCommit.Get();
	SelectAllTextOnCommit = Defaults._SelectAllTextOnCommit.Get();

	WidgetStyle = *Defaults._Style;
	ForegroundColor = FSlateColor(FLinearColor::Black);
}

void UIntSpinBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyIntSpinBox.Reset();
}

TSharedRef<SWidget> UIntSpinBox::RebuildWidget()
{
	MyIntSpinBox = SNew(SSpinBox<int32>)
	.Style(&WidgetStyle)
	.Font(Font)
	.ClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit)
	.SelectAllTextOnCommit(SelectAllTextOnCommit)
	.Justification(Justification)
	.OnValueChanged(BIND_UOBJECT_DELEGATE(FOnInt32ValueChanged, HandleOnValueChanged))
	.OnValueCommitted(BIND_UOBJECT_DELEGATE(FOnInt32ValueCommitted, HandleOnValueCommitted))
	.OnBeginSliderMovement(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnBeginSliderMovement))
	.OnEndSliderMovement(BIND_UOBJECT_DELEGATE(FOnInt32ValueChanged, HandleOnEndSliderMovement))
	;

	return MyIntSpinBox.ToSharedRef();
}

void UIntSpinBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyIntSpinBox->SetDelta(Delta);
	MyIntSpinBox->SetSliderExponent(SliderExponent);
	MyIntSpinBox->SetMinDesiredWidth(MinDesiredWidth);

	MyIntSpinBox->SetForegroundColor(ForegroundColor);

	// Set optional values
	bOverride_MinValue ? SetMinValue(MinValue) : ClearMinValue();
	bOverride_MaxValue ? SetMaxValue(MaxValue) : ClearMaxValue();
	bOverride_MinSliderValue ? SetMinSliderValue(MinSliderValue) : ClearMinSliderValue();
	bOverride_MaxSliderValue ? SetMaxSliderValue(MaxSliderValue) : ClearMaxSliderValue();

	// Always set the value last so that the max/min values are taken into account.
	TAttribute<int32> ValueBinding = PROPERTY_BINDING(int32, Value);
	MyIntSpinBox->SetValue(ValueBinding);
}

int32 UIntSpinBox::GetValue() const
{
	if (MyIntSpinBox.IsValid())
	{
		return MyIntSpinBox->GetValue();
	}

	return Value;
}

void UIntSpinBox::SetValue(int32 InValue)
{
	Value = InValue;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetValue(InValue);
	}
}

// MIN VALUE
int32 UIntSpinBox::GetMinValue() const
{
	int32 ReturnVal = TNumericLimits<int32>::Lowest();

	if (MyIntSpinBox.IsValid())
	{
		ReturnVal = MyIntSpinBox->GetMinValue();
	}
	else if (bOverride_MinValue)
	{
		ReturnVal = MinValue;
	}

	return ReturnVal;
}

void UIntSpinBox::SetMinValue(int32 InMinValue)
{
	bOverride_MinValue = true;
	MinValue = InMinValue;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMinValue(InMinValue);
	}
}

void UIntSpinBox::ClearMinValue()
{
	bOverride_MinValue = false;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMinValue(TOptional<int32>());
	}
}

// MAX VALUE
int32 UIntSpinBox::GetMaxValue() const
{
	int32 ReturnVal = TNumericLimits<int32>::Max();

	if (MyIntSpinBox.IsValid())
	{
		ReturnVal = MyIntSpinBox->GetMaxValue();
	}
	else if (bOverride_MaxValue)
	{
		ReturnVal = MaxValue;
	}

	return ReturnVal;
}

void UIntSpinBox::SetMaxValue(int32 InMaxValue)
{
	bOverride_MaxValue = true;
	MaxValue = InMaxValue;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMaxValue(InMaxValue);
	}
}
void UIntSpinBox::ClearMaxValue()
{
	bOverride_MaxValue = false;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMaxValue(TOptional<int32>());
	}
}

// MIN SLIDER VALUE
int32 UIntSpinBox::GetMinSliderValue() const
{
	int32 ReturnVal = TNumericLimits<int32>::Min();

	if (MyIntSpinBox.IsValid())
	{
		ReturnVal = MyIntSpinBox->GetMinSliderValue();
	}
	else if (bOverride_MinSliderValue)
	{
		ReturnVal = MinSliderValue;
	}

	return ReturnVal;
}

void UIntSpinBox::SetMinSliderValue(int32 InMinSliderValue)
{
	bOverride_MinSliderValue = true;
	MinSliderValue = InMinSliderValue;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMinSliderValue(InMinSliderValue);
	}
}

void UIntSpinBox::ClearMinSliderValue()
{
	bOverride_MinSliderValue = false;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMinSliderValue(TOptional<int32>());
	}
}

// MAX SLIDER VALUE
int32 UIntSpinBox::GetMaxSliderValue() const
{
	int32 ReturnVal = TNumericLimits<int32>::Max();

	if (MyIntSpinBox.IsValid())
	{
		ReturnVal = MyIntSpinBox->GetMaxSliderValue();
	}
	else if (bOverride_MaxSliderValue)
	{
		ReturnVal = MaxSliderValue;
	}

	return ReturnVal;
}

void UIntSpinBox::SetMaxSliderValue(int32 InMaxSliderValue)
{
	bOverride_MaxSliderValue = true;
	MaxSliderValue = InMaxSliderValue;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMaxSliderValue(InMaxSliderValue);
	}
}

void UIntSpinBox::ClearMaxSliderValue()
{
	bOverride_MaxSliderValue = false;
	if (MyIntSpinBox.IsValid())
	{
		MyIntSpinBox->SetMaxSliderValue(TOptional<int32>());
	}
}

void UIntSpinBox::SetForegroundColor(FSlateColor InForegroundColor)
{
	ForegroundColor = InForegroundColor;
	if ( MyIntSpinBox.IsValid() )
	{
		MyIntSpinBox->SetForegroundColor(ForegroundColor);
	}
}

// Event handlers
void UIntSpinBox::HandleOnValueChanged(int32 InValue)
{
	if ( !IsDesignTime() )
	{
		OnValueChanged.Broadcast(InValue);
	}
}

void UIntSpinBox::HandleOnValueCommitted(int32 InValue, ETextCommit::Type CommitMethod)
{
	if ( !IsDesignTime() )
	{
		OnValueCommitted.Broadcast(InValue, CommitMethod);
	}
}

void UIntSpinBox::HandleOnBeginSliderMovement()
{
	if ( !IsDesignTime() )
	{
		OnBeginSliderMovement.Broadcast();
	}
}

void UIntSpinBox::HandleOnEndSliderMovement(int32 InValue)
{
	if ( !IsDesignTime() )
	{
		OnEndSliderMovement.Broadcast(InValue);
	}
}

void UIntSpinBox::PostLoad()
{
	Super::PostLoad();

	if ( GetLinkerUEVersion() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS )
	{
		if ( Style_DEPRECATED != nullptr )
		{
			const FSpinBoxStyle* StylePtr = Style_DEPRECATED->GetStyle<FSpinBoxStyle>();
			if ( StylePtr != nullptr )
			{
				WidgetStyle = *StylePtr;
			}

			Style_DEPRECATED = nullptr;
		}
	}
}


#if WITH_EDITOR

const FText UIntSpinBox::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
