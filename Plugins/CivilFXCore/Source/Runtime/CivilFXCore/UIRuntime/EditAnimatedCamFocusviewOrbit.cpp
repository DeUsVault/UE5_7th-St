// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCamFocusviewOrbit.h"

#include "Components/CheckBox.h"
#include "Components/BackgroundBlur.h"

#include "Components/SpinBox.h"
#include "Components/TextBlock.h"

#include "TextableButton.h"
#include "EditAnimatedCameraOverviewButton.h"

#include "CivilFXCore/CommonCore/CivilFXPawn.h"

#include "Kismet/KismetMathLibrary.h"

void UEditAnimatedCamFocusviewOrbit::NativeConstruct()
{
	Super::NativeConstruct();

	EnableEditCheckBox->OnCheckStateChanged.AddDynamic(this, &UEditAnimatedCamFocusviewOrbit::OnOrbitCheckBoxChanged);
	HeightSpinBox->OnValueChanged.AddDynamic(this, &UEditAnimatedCamFocusviewOrbit::OnOrbitHeightSpinBoxChanged);
	RadiusSpinBox->OnValueChanged.AddDynamic(this, &UEditAnimatedCamFocusviewOrbit::OnOrbitRadiusSpinBoxChanged);
	CaptureLocationButton->GetButton()->OnClicked.AddDynamic(this, &UEditAnimatedCamFocusviewOrbit::OnCaptureLocationButtonClicked);
	ReCaptureLocationButton->GetButton()->OnClicked.AddDynamic(this, &UEditAnimatedCamFocusviewOrbit::OnReCaptureLocationButtonClicked);
	GenerateViewButton->GetButton()->OnClicked.AddDynamic(this, &UEditAnimatedCamFocusviewOrbit::OnGenerateViewButtonClicked);
}

void UEditAnimatedCamFocusviewOrbit::NativeDestruct()
{
	Super::NativeDestruct();

	if (bOrbitEditing)
	{
		PlayerPawn->OnEndCreateOrbitObject();
	}
}

void UEditAnimatedCamFocusviewOrbit::RefreshPanel(UEditAnimatedCameraOverviewButton* InEditingButton)
{
	Super::RefreshPanel(InEditingButton);

	HeightSpinBox->SetValue(CameraNodeData->Height);
	RadiusSpinBox->SetValue(CameraNodeData->XRadius);

	/** Rebuild Camera Data here */
	PlayerPawn->OnBeginCameraMovement(CameraNodeData->Locations, CameraNodeData->Rotations, CameraNodeData->Duration, 0, true);
}

void UEditAnimatedCamFocusviewOrbit::OnOrbitCheckBoxChanged(bool bIsChecked)
{
	if (bIsChecked)
	{
		OrbitPanelBackgroundBlur->SetVisibility(ESlateVisibility::Hidden);
		PlayerPawn->OnBeginCreateOrbitObject(OrbitObject, CameraNodeData->CenterLocation);
		PlayerPawn->UpdateOrbitRadiusAndHeight(CameraNodeData->Height, CameraNodeData->XRadius);
		PlayerPawn->UpdateOrbitViewLocations(CameraNodeData->Locations);
		if (CameraNodeData->bHasLoadedData)
		{
			OnCaptureLocationButtonClicked();
		}
		else
		{
			OnReCaptureLocationButtonClicked();
		}

	}
	else
	{
		OrbitPanelBackgroundBlur->SetVisibility(ESlateVisibility::Visible);
		PlayerPawn->OnEndCreateOrbitObject();
	}
	bOrbitEditing = bIsChecked;
	/** */

}

void UEditAnimatedCamFocusviewOrbit::OnOrbitHeightSpinBoxChanged(float InValue)
{
	OrbitHeight = InValue;
}

void UEditAnimatedCamFocusviewOrbit::OnOrbitRadiusSpinBoxChanged(float InValue)
{
	OrbitRadius = InValue;
}

void UEditAnimatedCamFocusviewOrbit::OnGenerateViewButtonClicked()
{
	GenerateLocationsAndRotations();

	GenerateViewButton->OnDeselected();
}

void UEditAnimatedCamFocusviewOrbit::OnCaptureLocationButtonClicked()
{
	CaptureLocationButton->SetVisibility(ESlateVisibility::Hidden);
	ReCaptureLocationButton->SetVisibility(ESlateVisibility::Visible);

	PlayerPawn->bDrawingOrbitObjectLocation = false;

	CaptureLocationButton->OnDeselected();
}

void UEditAnimatedCamFocusviewOrbit::OnReCaptureLocationButtonClicked()
{
	CaptureLocationButton->SetVisibility(ESlateVisibility::Visible);
	ReCaptureLocationButton->SetVisibility(ESlateVisibility::Hidden);

	PlayerPawn->bDrawingOrbitObjectLocation = true;

	ReCaptureLocationButton->OnDeselected();
}

void UEditAnimatedCamFocusviewOrbit::GenerateLocationsAndRotations()
{
	CameraNodeData->CenterLocation = PlayerPawn->GetOrbitObjectLocation();
	CameraNodeData->Locations = GetEclipsePath(OrbitRadius, OrbitRadius, CameraNodeData->CenterLocation, OrbitHeight);
	CameraNodeData->Rotations.Empty();
	for (const FVector& Location : CameraNodeData->Locations)
	{
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Location, CameraNodeData->CenterLocation);
		CameraNodeData->Rotations.Add(Rotation);
	}
	CameraNodeData->Height = OrbitHeight;
	CameraNodeData->XRadius = OrbitRadius;
	CameraNodeData->YRadius = OrbitRadius;


	PlayerPawn->UpdateOrbitRadiusAndHeight(OrbitHeight, OrbitRadius);
	PlayerPawn->UpdateOrbitViewLocations(CameraNodeData->Locations);

	EditingButton->GenerateThumbnail(CameraNodeData->Locations[0], CameraNodeData->Rotations[0]);

	/** Refresh camera data for preview */
	PlayerPawn->OnBeginCameraMovement(CameraNodeData->Locations, CameraNodeData->Rotations, CameraNodeData->Duration, 0,true);
}

TArray<FVector> UEditAnimatedCamFocusviewOrbit::GetEclipsePath(float YRadius, float XRadius, FVector Origin, float Height, float Theta/*=0.f*/, int32 Resolution/*=12*/)
{
	TArray<FVector> Result;
	Result.Reserve(Resolution + 1);
	FVector Angle;
	Angle = Angle.RotateAngleAxis(Theta, FVector::ForwardVector);

	FVector Center = Origin;
	Center.Z += Height;
	for (int32 i = 0; i <= Resolution; ++i)
	{
		float FloatAngle = (float)i / (float)Resolution * 2.0f * UKismetMathLibrary::GetPI();
		/*
		Result[i] = FVector(ZRadius * FMath::Sin(FloatAngle) ,YRadius * FMath::Cos(FloatAngle), 0.f);
		Result[i] = Angle * Result[i] + Center;
		*/
		FVector NewLocation = FVector(XRadius * FMath::Sin(FloatAngle), YRadius * FMath::Cos(FloatAngle), 0.f);
		NewLocation = NewLocation + Center;
		Result.Add(NewLocation);

	}
	return Result;
}
/*
private Vector3[] GetEclipse(float xRadius, float zRadius, Vector3 origin, float height, float theta, int resolution)
{
	Vector3[] positions = new Vector3[resolution + 1];
	Quaternion q = Quaternion.AngleAxis(theta, Vector3.forward);
	Vector3 center = origin;
	center.y += height;

	//horizonal
	for (int i = 0; i <= resolution; i++)
	{
		float angle = (float)i / (float)resolution * 2.0f * Mathf.PI;
		positions[i] = new Vector3(xRadius * Mathf.Cos(angle), 0.0f, zRadius * Mathf.Sin(angle));
		positions[i] = q * positions[i] + center;
	}

	return positions;
}
*/