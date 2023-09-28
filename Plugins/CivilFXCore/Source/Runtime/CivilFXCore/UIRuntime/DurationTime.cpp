// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "DurationTime.h"

#include "Components/EditableText.h"
#include "Widgets/Input/SEditableText.h"

void UDurationTime::NativeConstruct()
{
	MinutesInput->OnTextCommitted.AddDynamic(this, &UDurationTime::HandleTextInputCommitted);
	SecondsInput->OnTextCommitted.AddDynamic(this, &UDurationTime::HandleTextInputCommitted);

	CachedDuration = GetDuration();
}

float UDurationTime::GetDuration() const
{
	int32 Minutes = FCString::Atoi(*MinutesInput->GetText().ToString());
	int32 Seconds = FCString::Atoi(*SecondsInput->GetText().ToString());
	return Minutes * 60.0f + Seconds;
}

void UDurationTime::SetDuration(float Duration)
{
	CachedDuration = Duration;

	int32 DurationAsInt = FMath::CeilToInt(Duration);
	int32 Minutes = DurationAsInt / 60; //integer math
	int32 Seconds = DurationAsInt % 60;

	MinutesInput->SetText(FText::FromString(FString::Printf(TEXT("%02d"), Minutes)));
	SecondsInput->SetText(FText::FromString(FString::Printf(TEXT("%02d"), Seconds)));
}

void UDurationTime::SetDuration(int32 Minutes, int32 Seconds)
{
	SetDuration(Minutes * 60.f + Seconds);
}

void UDurationTime::SetInteractable(bool bInteractable)
{
	FLinearColor MinutesColor = MinutesInput->WidgetStyle.ColorAndOpacity.GetSpecifiedColor();
	FLinearColor SecondsColor = SecondsInput->WidgetStyle.ColorAndOpacity.GetSpecifiedColor();

	if (bInteractable)
	{
		MinutesColor.A = 1.0f;
		SecondsColor.A = 1.0f;
	}
	else
	{
		MinutesColor.A = 0.3f;
		SecondsColor.A = 0.3f;
	}

	TSharedPtr<SEditableText> MinutesWidget = StaticCastSharedPtr<SEditableText>(MinutesInput->GetCachedWidget());
	MinutesWidget->SetColorAndOpacity(FSlateColor(MinutesColor));

	TSharedPtr<SEditableText> SecondsWidget = StaticCastSharedPtr<SEditableText>(SecondsInput->GetCachedWidget());
	SecondsWidget->SetColorAndOpacity(FSlateColor(SecondsColor));

	MinutesInput->SetIsReadOnly(!bInteractable);
	SecondsInput->SetIsReadOnly(!bInteractable);
}

void UDurationTime::HandleTextInputCommitted(const FText& InText, ETextCommit::Type CommitedType)
{
	FString TempString = InText.ToString();

	//replacing +- to invalid numeric character ('A')
	TempString.ReplaceInline(TEXT("+-"), TEXT("A"));

	if (TempString.IsNumeric())
	{
		CachedDuration = GetDuration();
	}

	SetDuration(CachedDuration);
	OnDurationValueChanged.ExecuteIfBound(CachedDuration);
}
