// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraManualButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UEditAnimatedCameraManualButton::NativeConstruct()
{
	Super::NativeConstruct();

	MainButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraManualButton::OnMainButtonClicked);

	DeleteButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraManualButton::OnDeleteButtonClicked);
	RefreshButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraManualButton::OnRefreshButtonClicked);
	AddLeftButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraManualButton::OnAddLeftButtonClicked);
	AddRightButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraManualButton::OnAddRightButtonClicked);
}

void UEditAnimatedCameraManualButton::OnCreated(const FVector& InLocation, const FRotator& InRotation)
{
	Location = InLocation;
	Rotation = InRotation;
	GenerateThumbnail(Location, Rotation);
}

void UEditAnimatedCameraManualButton::GetView(FVector& OutLocation, FRotator& OutRotation)
{
	OutLocation = Location;
	OutRotation = Rotation;
}

void UEditAnimatedCameraManualButton::SetView(const FVector& InLocation, const FRotator& InRotation)
{
	Location = InLocation;
	Rotation = InRotation;
}

void UEditAnimatedCameraManualButton::UpdateCurrentIndex(int32 InIndex, int32 ChildrenNum)
{
	CurrentIndex = InIndex;
	DebugTextBlock->SetText(FText::FromString(FString::FromInt(CurrentIndex)));
	ESlateVisibility SlateVisibility = (CurrentIndex == 0) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	SetAddLeftButtonVisibility(SlateVisibility);
}	

int32 UEditAnimatedCameraManualButton::GetCurrentIndex() const
{
	return CurrentIndex;
}

void UEditAnimatedCameraManualButton::SetAddLeftButtonVisibility(ESlateVisibility SlateVisibility)
{
	AddLeftButton->SetVisibility(SlateVisibility);
}

void UEditAnimatedCameraManualButton::OnMainButtonClicked()
{
	GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(Location);
	GetWorld()->GetFirstPlayerController()->SetControlRotation(Rotation);
}

void UEditAnimatedCameraManualButton::OnDeleteButtonClicked()
{
	if (CurrentIndex == 0)
	{
		return;
	}
	OnRequestRemoveChild.Broadcast(CurrentIndex);
	RemoveFromParent();
}

void UEditAnimatedCameraManualButton::OnRefreshButtonClicked()
{
	GetWorld()->GetFirstPlayerController()->GetActorEyesViewPoint(Location, Rotation);
	GenerateThumbnail(Location, Rotation);
	OnRequestRefreshChild.Broadcast(CurrentIndex);
	GLog->Log(Location.ToString());
}

void UEditAnimatedCameraManualButton::OnAddLeftButtonClicked()
{
	OnRequestAddChild.Broadcast(CurrentIndex);
}

void UEditAnimatedCameraManualButton::OnAddRightButtonClicked()
{
	OnRequestAddChild.Broadcast(CurrentIndex + 1);
}
