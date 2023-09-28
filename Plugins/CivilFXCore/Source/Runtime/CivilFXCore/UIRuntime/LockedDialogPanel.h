// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "DialogPanel.h"
#include "LockedDialogPanel.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API ULockedDialogPanel : public UDialogPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void OnConfirm() override;
	virtual void RemovePanel() override;

	void SetContent(const FString& InContent);
	/**
	*
	@return: Weak pointer because the panel is destroyed from within
	*/
	static ULockedDialogPanel* CurrentPanel;
	static TWeakObjectPtr<ULockedDialogPanel> CreateAndAddToViewport(UWorld* WorldContext, const FString& InTitle = FString(), const FString& InHintText = FString());

protected:
	UPROPERTY(VisibleAnywhere, Category = "Dialog Panel Settings", meta = (BindWidget))
	class UTextBlock* ContentBox;
	UPROPERTY(VisibleAnywhere, Category = "Dialog Panel Settings", meta = (BindWidget))
	class UBorder* Container;
private:
	FTimerHandle Timer;
	UFUNCTION()
	FEventReply HandleMouseLocked(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	void MoveMouseCursorToWidget();
};
