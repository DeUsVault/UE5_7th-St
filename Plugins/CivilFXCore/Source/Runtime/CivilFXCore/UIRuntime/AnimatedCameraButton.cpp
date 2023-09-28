// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "AnimatedCameraButton.h"
#include "TextableButton.h"
#include "ChevronCreator.h"
#include "CivilFXCore/CommonCore/CivilFXPawn.h"


void UAnimatedCameraButton::NativeConstruct()
{

	MainButton->GetButton()->OnClicked.AddDynamic(this, &UAnimatedCameraButton::OnMainButtonClicked);

	PreviewButton->OnClicked.AddDynamic(this, &UAnimatedCameraButton::OnPreviewButtonClicked);

	PlayerPawn = Cast<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UAnimatedCameraButton::NativeDestruct()
{
	if (ChevronActor)
	{
		ChevronActor->Destroy(false, false);
	}
}

void UAnimatedCameraButton::SetCameraNodeData(const FAnimatedCameraNodeData& InCameraData)
{
	CameraNodeData = InCameraData;

	if (ChevronActor)
	{
		ChevronActor->Destroy(false, false);
		ChevronActor = nullptr;
	}

	GLog->Log("SetCameraNodeData");
	if (CameraNodeData.IsMovementType())
	{
		GLog->Log("ChevronActor");
		PreviewButton->SetVisibility(ESlateVisibility::Visible);
		//rebuild chevron actor
		ChevronActor = GetWorld()->SpawnActor<AChevronCreator>();
		ChevronActor->BuildChevronMesh(CameraNodeData.Locations, FLinearColor(CameraNodeData.CenterLocation), CameraNodeData.YRadius, CameraNodeData.Size);
	}
}

void UAnimatedCameraButton::SetCameraButtonName(FText InCameraName)
{
	MainButton->SetButtonText(InCameraName);
}

void UAnimatedCameraButton::StopChevron()
{
	if (ChevronActor)
	{
		ChevronActor->SetActorHiddenInGame(true);
	}
}

void UAnimatedCameraButton::OnMainButtonClicked()
{
	const float Height = CameraNodeData.CameraType.Equals(TEXT("Movement")) ? CameraNodeData.Height : 0;
	PlayerPawn->OnBeginCameraMovement(CameraNodeData.Locations, CameraNodeData.Rotations, CameraNodeData.Duration, Height);
}

void UAnimatedCameraButton::OnPreviewButtonClicked()
{
	static bool bHidden = true;
	bHidden = !bHidden;

	ChevronActor->SetActorHiddenInGame(bHidden);

	PreviewButton->WidgetStyle.Normal.TintColor = bHidden ? FSlateColor(FLinearColor::White) : FSlateColor(FLinearColor::Blue);
}
