// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "StillCameraButton.h"
#include "Components/Button.h"
#include "GameFramework/Actor.h"
#include "TextableButton.h"
#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "CamerasDataParserHelper.h"

void UStillCameraButton::NativeConstruct()
{
	/*
	FScriptDelegate MainDelegate;
	MainDelegate.BindUFunction(this, FName("OnMainButtonClicked"));
	MainButton->GetButton()->OnClicked.Add(MainDelegate);
	*/
	MainButton->GetButton()->OnClicked.AddDynamic(this, &UStillCameraButton::OnMainButtonClicked);

	/*
	FScriptDelegate DeleteDelegate;
	DeleteDelegate.BindUFunction(this, FName("OnDeleteButtonClicked"));
	DeleteButton->OnClicked.Add(DeleteDelegate);
	*/
	DeleteButton->OnClicked.AddDynamic(this, &UStillCameraButton::OnDeleteButtonClicked);

	PlayerPawn = CastChecked<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
}



void UStillCameraButton::SetCameraButtonName(FText InCameraName)
{
	CameraName = InCameraName;
	MainButton->SetButtonText(InCameraName);
}

FString UStillCameraButton::GetSerializedString(int32 Indent) const
{
	/*
	CameraName:View 1
	{
		Location
		{
			X:-802.500793
			Y:1650.300537
			Z:490.263000
		}
		Rotation
		{
			X:0.000000
			Y:-22.194391
			Z:-63.277500
		}
	}*/

	FString Result;
	FCamerasDataParserHelper::AppendTab(Result, Indent);
	Result.Append(FString::Format(TEXT("{0}:{1}{2}"), {TEXT("CameraName"), CameraName.ToString(), FCamerasDataParserHelper::Newline}));
	FCamerasDataParserHelper::AppendTab(Result, Indent);
	Result.Append(FCamerasDataParserHelper::SMNL);
	FCamerasDataParserHelper::BuildLocationString(Result, {Location}, Indent + 1);
	FCamerasDataParserHelper::BuildRotationString(Result, {Rotation}, Indent + 1);
	FCamerasDataParserHelper::AppendTab(Result, Indent);
	Result.Append(FCamerasDataParserHelper::EMNL);
	return Result;
}



void UStillCameraButton::OnMainButtonClicked()
{
	check(bHasTransformData);
	PlayerPawn->OnBeginCameraMovement(Location, Rotation, 3.0f);
}

void UStillCameraButton::OnDeleteButtonClicked()
{
	RemoveFromParent();
}

void UStillCameraButton::SetCameraLocationAndRotation(const FVector& InLocation, const FRotator& InRotation)
{
	Location = InLocation;
	Rotation = InRotation;
	bHasTransformData = true;
}

