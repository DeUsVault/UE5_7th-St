// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DurationTime.generated.h"

class UEditableText;

UCLASS()
class CIVILFXCORE_API UDurationTime : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDurationValueChanged, float, Duration);

public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	float GetDuration() const;
	void SetDuration(float);
	void SetDuration(int32 Minutes, int32 Seconds);
	void SetInteractable(bool bInteractable);
	FOnDurationValueChanged OnDurationValueChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Duration Time Settings", meta = (BindWidget))
	UEditableText* MinutesInput;
	UPROPERTY(VisibleInstanceOnly, Category = "Duration Time Settings", meta = (BindWidget))
	UEditableText* SecondsInput;

private:
	UFUNCTION()
	void HandleTextInputCommitted(const FText& InText, ETextCommit::Type CommitedType);

	float CachedDuration;
};
