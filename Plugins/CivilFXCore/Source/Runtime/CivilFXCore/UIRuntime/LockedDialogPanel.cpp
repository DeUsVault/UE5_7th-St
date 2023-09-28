// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "LockedDialogPanel.h"

#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ULockedDialogPanel* ULockedDialogPanel::CurrentPanel = nullptr;

void ULockedDialogPanel::NativeConstruct()
{
	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ULockedDialogPanel::MoveMouseCursorToWidget);
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ULockedDialogPanel::MoveMouseCursorToWidget, 0.1f, false);

	Container->OnMouseMoveEvent.BindDynamic(this, &ULockedDialogPanel::HandleMouseLocked);

	Super::NativeConstruct();
}

void ULockedDialogPanel::OnConfirm()
{
	DialogConfirmedDelegate.ExecuteIfBound(LastInputText);
	RemovePanel();
}

void ULockedDialogPanel::RemovePanel()
{
	CurrentPanel = nullptr;
	Super::RemovePanel();
}

TWeakObjectPtr<ULockedDialogPanel> ULockedDialogPanel::CreateAndAddToViewport(UWorld* WorldContext, const FString& InTitle /*= FString()*/, const FString& InHintText /*= FString()*/)
{
	if (CurrentPanel)
	{
		CurrentPanel->OnCancel();
	}

	TSubclassOf<ULockedDialogPanel> PanelClass = LoadClass<ULockedDialogPanel>(nullptr, TEXT("WidgetBlueprint'/CivilFXCore/UI_BPs/BP_LockedDialogPanel.BP_LockedDialogPanel_C'"));
	check(PanelClass);
	ULockedDialogPanel* Widget = Cast<ULockedDialogPanel>(CreateWidget(WorldContext, PanelClass));
	check(Widget);
	Widget->SetTitle(InTitle);
	Widget->SetInputHint(InHintText);
	Widget->AddToViewport();

	CurrentPanel = Widget;

	return Widget;
}


void ULockedDialogPanel::SetContent(const FString& InContent)
{
	ContentBox->SetText(FText::FromString(InContent));
}

/*
void ULockedDialogPanel::HandleMouseLocked()
{
	FVector2D WidgetSize = GetCachedGeometry().GetLocalSize();
	GLog->Log(FString::Printf(TEXT("WidgetSize: %s"), *WidgetSize.ToString()));
	//get screen mouse position
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	FVector2D MousePosition;
	PC->GetMousePosition(MousePosition.X, MousePosition.Y);
	GLog->Log(FString::Printf(TEXT("MousePosition: %s"), *MousePosition.ToString()));

	FVector2D ScreenPos;
	USlateBlueprintLibrary::ScreenToWidgetLocal(GetWorld(), GetCachedGeometry(), MousePosition, ScreenPos);
	GLog->Log(FString::Printf(TEXT("ScreenPos: %s"), *ScreenPos.ToString()));
	
	FVector2D Pix;
	FVector2D Pos;
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), GetCachedGeometry(), ScreenPos, Pix, Pos);
	GLog->Log(FString::Printf(TEXT("Pixel: %s"), *Pix.ToString()));
	GLog->Log(FString::Printf(TEXT("Pos: %s"), *Pos.ToString()));

	if (ScreenPos.X < 0)
	{
		MousePosition.X = Pix.X + ScreenPos.X;
	}

	if (ScreenPos.X > WidgetSize.Y)
	{
		MousePosition.X = Pix.X - ScreenPos.X;
	}

	if (ScreenPos.Y < 0 || ScreenPos.Y > WidgetSize.Y)
	{
		MousePosition.Y = Pix.Y;
	}
	
	PC->SetMouseLocation(MousePosition.X, MousePosition.Y);
}
*/

FEventReply ULockedDialogPanel::HandleMouseLocked(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply = UWidgetBlueprintLibrary::Handled();
	UWidgetBlueprintLibrary::LockMouse(Reply, Container);
	//GLog->Log("Locking Mouse");
	return Reply;
}

void ULockedDialogPanel::MoveMouseCursorToWidget()
{
	FVector2D WidgetSize = Container->GetCachedGeometry().GetLocalSize();

	//get screen mouse position
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	FVector2D MousePosition;
	PC->GetMousePosition(MousePosition.X, MousePosition.Y);

	FVector2D ScreenPos;
	USlateBlueprintLibrary::ScreenToWidgetLocal(GetWorld(), Container->GetCachedGeometry(), MousePosition, ScreenPos);
	GLog->Log(FString::Printf(TEXT("ScreenPos: %s"), *ScreenPos.ToString()));

	if (ScreenPos >= FVector2D(-1, -1) && ScreenPos <= WidgetSize)
	{

	}
	else
	{
		ScreenPos = Container->GetCachedGeometry().GetLocalSize() / 2;
		FVector2D Pix;
		FVector2D Pos;
		USlateBlueprintLibrary::LocalToViewport(GetWorld(), Container->GetCachedGeometry(), ScreenPos, Pix, Pos);
		PC->SetMouseLocation(Pix.X, Pix.Y);
	}
	


}
