// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCamFocusviewManual.h"
#include "EditAnimatedCameraOverviewButton.h"

#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/Slider.h"

#include "CivilFXCore/CommonCore/CivilFXPawn.h"

void UEditAnimatedCamFocusviewManual::RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton)
{
	Super::RefreshPanel(InEditingButton);

	GLog->Log(FString::Printf(TEXT("Refreshing Panel for Camera: %s"), *CameraNodeData->CameraName));

	TempLocations = CameraNodeData->Locations;
	TempRotations = CameraNodeData->Rotations;

	//remove all container
	for (UWidget* Child : ContainerScrollbox->GetAllChildren())
	{
		Child->RemoveFromParent();
	}

	if (!CameraNodeData->bHasLoadedData)
	{
		FVector Location;
		FRotator Rotation;
		GetWorld()->GetFirstPlayerController()->GetActorEyesViewPoint(Location, Rotation);
		AddChildToContainer(Location, Rotation);
		CameraNodeData->bHasLoadedData = true;
	}
	else
	{
		for (int32 i = 0; i < TempLocations.Num(); ++i)
		{
			AddChildToContainer(TempLocations[i], TempRotations[i]);
		}
	}
	UpdateChildrenCurrentIndex();

	/** Rebuild Camera Data here */
	PlayerPawn->OnBeginCameraMovement(TempLocations, TempRotations, CameraNodeData->Duration, 0, true);
}

void UEditAnimatedCamFocusviewManual::AddChildToContainer(const FVector& InLocation, const FRotator& InRotation)
{
	UWidget* Child = CreateWidget(GetWorld(), EditAnimatedCameraManualButtonClass);
	check(Child);

	ContainerScrollbox->AddChild(Child);
	
	UEditAnimatedCameraManualButton* ChildButton = Cast<UEditAnimatedCameraManualButton>(Child);
	ChildButton->OnCreated(InLocation, InRotation);
	ChildButton->OnRequestAddChild.AddUObject(this, &UEditAnimatedCamFocusviewManual::OnRequestAddingNewChild);
	ChildButton->OnRequestRemoveChild.AddUObject(this, &UEditAnimatedCamFocusviewManual::OnRequestRemoveChild);
	ChildButton->OnRequestRefreshChild.AddUObject(this, &UEditAnimatedCamFocusviewManual::OnRequestRefreshChild);
	ChildButton->MainButton->OnClicked.AddDynamic(this, &UEditAnimatedCamFocusviewManual::OnChildMainButtonClicked);
}

void UEditAnimatedCamFocusviewManual::OnTimelineSliderChanged(float InValue)
{
	Super::OnTimelineSliderChanged(InValue);
	//PlayerPawn->OnBeginPreviewAtTimeline(InValue);
	if (CurrentSelectedButton)
	{
		CurrentSelectedButton->OnDeselected();
		CurrentSelectedButton = nullptr;
	}
}

void UEditAnimatedCamFocusviewManual::OnRequestAddingNewChild(int32 Index)
{
	FVector Location;
	FRotator Rotation;
	GetWorld()->GetFirstPlayerController()->GetActorEyesViewPoint(Location, Rotation);
	
	AddChildToContainer(Location, Rotation);

	TempLocations.Insert(Location, Index);
	TempRotations.Insert(Rotation, Index);

	UpdateChildrenCurrentIndex();

	/** Rebuild Camera Data here */
	PlayerPawn->OnBeginCameraMovement(TempLocations, TempRotations, CameraNodeData->Duration, 0, true);
}



void UEditAnimatedCamFocusviewManual::OnRequestRemoveChild(int32 Index)
{
	TempLocations.RemoveAt(Index);
	TempRotations.RemoveAt(Index);

	//the widget should be deleted itself.
	//update all information except for the widget.
	//it's okay because we update this in the next frame anyway.
	UpdateChildrenCurrentIndex();

	/** Rebuild Camera Data here */
	PlayerPawn->OnBeginCameraMovement(TempLocations, TempRotations, CameraNodeData->Duration, 0, true);
}

void UEditAnimatedCamFocusviewManual::OnRequestRefreshChild(int32 Index)
{
	FVector Location;
	FRotator Rotation;
	if (UEditAnimatedCameraManualButton* ChildButton = Cast<UEditAnimatedCameraManualButton>(ContainerScrollbox->GetChildAt(Index)))
	{
		ChildButton->GetView(Location, Rotation);
		TempLocations[Index] = Location;
		TempRotations[Index] = Rotation;

		CameraNodeData->Locations = TempLocations;
		CameraNodeData->Rotations = TempRotations;
	}

	/** Rebuild Camera Data here */
	PlayerPawn->OnBeginCameraMovement(TempLocations, TempRotations, CameraNodeData->Duration, 0, true);
}

void UEditAnimatedCamFocusviewManual::UpdateChildrenCurrentIndex()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		int32 ChildrenNum = ContainerScrollbox->GetChildrenCount();

		//while we're here just update the CameraNodeData.
		//TODO: do it somewhere else
		int32 Index = 0;
		GLog->Log(FString::Printf(TEXT("UpdateChildrenCurrentIndex: %d"), ChildrenNum));
		for (UWidget* ChildWidget : ContainerScrollbox->GetAllChildren())
		{
			if (UEditAnimatedCameraManualButton* ChildButton = Cast<UEditAnimatedCameraManualButton>(ChildWidget))
			{
				//FVector Location;
				//FRotator Rotation;
				ChildButton->UpdateCurrentIndex(Index, ChildrenNum);
				ChildButton->SetView(TempLocations[Index], TempRotations[Index]);
				ChildButton->GenerateThumbnail(TempLocations[Index], TempRotations[Index]);
				//ChildButton->GetView(Location, Rotation);
				//Locations.Add(Location);
				//Rotations.Add(Rotation);
				++Index;
			}
		}
		
		GLog->Log(FString::Printf(TEXT("123: %d"), TempLocations.Num()));
		CameraNodeData->Locations = TempLocations;
		CameraNodeData->Rotations = TempRotations;
		GLog->Log(FString::Printf(TEXT("321: %d"), CameraNodeData->Locations.Num()));
		if (CameraNodeData->Locations.Num() == 0)
		{
			GLog->Log(FString::Printf(TEXT("Here: %s"), *CameraNodeData->CameraName));
		}
		else if (CameraNodeData->Rotations.Num() == 0)
		{
			GLog->Log(FString::Printf(TEXT("Here22: %s"), *CameraNodeData->CameraName));
		}
		else
		{
			EditingButton->GenerateThumbnail(CameraNodeData->Locations[0], CameraNodeData->Rotations[0]);
		}

	});
}

void UEditAnimatedCamFocusviewManual::OnChildMainButtonClicked()
{


	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		UEditAnimatedCameraManualButton* NewSelectedButton = nullptr;
		for (UWidget* Child : ContainerScrollbox->GetAllChildren())
		{
			UEditAnimatedCameraManualButton* ChildButton = Cast<UEditAnimatedCameraManualButton>(Child);

			if (ChildButton->IsButtonSelected())
			{
				GLog->Log("Should Always run");
				if (ChildButton != CurrentSelectedButton)
				{
					TimelineSlider->SetValue((float)ChildButton->GetCurrentIndex() / (ContainerScrollbox->GetAllChildren().Num() - 1));
					NewSelectedButton = ChildButton;
					break;
				}
		
			}

		}
		if (NewSelectedButton)
		{
			if (CurrentSelectedButton)
			{
				CurrentSelectedButton->OnDeselected();
			}
			CurrentSelectedButton = NewSelectedButton;
		}
	});
}
