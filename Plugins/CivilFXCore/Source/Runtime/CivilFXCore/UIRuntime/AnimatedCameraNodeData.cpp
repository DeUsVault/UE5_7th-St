// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "AnimatedCameraNodeData.h"
#include "CamerasDataParserHelper.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"

FAnimatedCameraNodeData::FAnimatedCameraNodeData() :
	CameraName(TEXT("<Camera Name>")),
	CameraType(TEXT("Orbit")),
	Duration(10),
	Category(TEXT("Default")),
	Phases{ UEnum::GetDisplayValueAsText(EPhaseType::Existing).ToString() },
	CenterLocation{FVector::ZeroVector},
	Height{1000.0f},
	XRadius{1000.0f},
	YRadius{1000.0},
	Size{0},
	Locations{FVector::ZeroVector},
	Rotations{FRotator::ZeroRotator},
	bHasLoadedData(false)
{
}

FString FAnimatedCameraNodeData::GetSerializedString(int32 Indent) const
{
	FString WriteString;

	FCamerasDataParserHelper::AppendTab(WriteString, Indent);
	WriteString.Append(FString::Format(TEXT("CameraName{0}{1}{2}"), {FCamerasDataParserHelper::Seperator, CameraName, FCamerasDataParserHelper::Newline}));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent);
	WriteString.Append(FCamerasDataParserHelper::SMNL);
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 1);
	WriteString.Append(TEXT("Properties"));
	WriteString.Append(FCamerasDataParserHelper::Newline);
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 1);
	WriteString.Append(FCamerasDataParserHelper::SMNL);
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 2);
	WriteString.Append(FString::Format(TEXT("Type{0}{1}{2}"), {FCamerasDataParserHelper::Seperator, CameraType, FCamerasDataParserHelper::Newline}));

	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 2);
	WriteString.Append(FCamerasDataParserHelper::SMNL);

	FCamerasDataParserHelper::BuildLocationString(WriteString, { CenterLocation }, Indent + 3);

	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 3);
	WriteString.Append(FString::Format(TEXT("XRadius{0}{1}{2}"), { FCamerasDataParserHelper::Seperator, XRadius, FCamerasDataParserHelper::Newline }));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 3);
	WriteString.Append(FString::Format(TEXT("YRadius{0}{1}{2}"), { FCamerasDataParserHelper::Seperator, YRadius, FCamerasDataParserHelper::Newline }));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 3);
	WriteString.Append(FString::Format(TEXT("Height{0}{1}{2}"), { FCamerasDataParserHelper::Seperator, Height, FCamerasDataParserHelper::Newline }));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 3);
	WriteString.Append(FString::Format(TEXT("Size{0}{1}{2}"), { FCamerasDataParserHelper::Seperator, Size, FCamerasDataParserHelper::Newline }));


	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 2);
	WriteString.Append(FCamerasDataParserHelper::EMNL);

	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 2);
	WriteString.Append(FString::Format(TEXT("Duration{0}{1}{2}"), {FCamerasDataParserHelper::Seperator, Duration, FCamerasDataParserHelper::Newline}));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 2);
	WriteString.Append(FString::Format(TEXT("Category{0}{1}{2}"), { FCamerasDataParserHelper::Seperator, Category, FCamerasDataParserHelper::Newline }));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 2);
	FString PhasesString;
	for (const FString& Phase : Phases)
	{
		PhasesString.Append(Phase);
		PhasesString.Append(FCamerasDataParserHelper::SubSeperator);
	}
	//remove the last ","
	PhasesString = PhasesString.Left(PhasesString.Len()-1);
	WriteString.Append(FString::Format(TEXT("Phase(s){0}{1}{2}"), { FCamerasDataParserHelper::Seperator, PhasesString, FCamerasDataParserHelper::Newline }));
	FCamerasDataParserHelper::AppendTab(WriteString, Indent + 1);
	WriteString.Append(FCamerasDataParserHelper::EMNL);

	FCamerasDataParserHelper::BuildLocationString(WriteString, Locations, Indent + 1);
	FCamerasDataParserHelper::BuildRotationString(WriteString, Rotations, Indent + 1);

	FCamerasDataParserHelper::AppendTab(WriteString, Indent);
	WriteString.Append(FCamerasDataParserHelper::EMNL);

	return WriteString;
}

bool FAnimatedCameraNodeData::SerializeDataFromString(TArray<FString>& InOutLines)
{
	Locations.Empty();
	Rotations.Empty();

	bool Success = FCamerasDataParserHelper::ParseValueToString(InOutLines, CameraName);
	Success &= FCamerasDataParserHelper::ParseMarker(InOutLines, FCamerasDataParserHelper::EMarkerType::StartMarker);
	if (!Success)
	{
		return false;
	}

	FString TempString;
	Success &= FCamerasDataParserHelper::ParseHeader(InOutLines, TempString);
	Success &= FCamerasDataParserHelper::ParseMarker(InOutLines, FCamerasDataParserHelper::EMarkerType::StartMarker);
	Success &= FCamerasDataParserHelper::ParseValueToString(InOutLines, CameraType);

	/** Orbit only data */
	Success &= FCamerasDataParserHelper::ParseMarker(InOutLines, FCamerasDataParserHelper::EMarkerType::StartMarker);
	TArray<FVector> Center;
	Success &= FCamerasDataParserHelper::ParseToLocations(InOutLines, Center);
	CenterLocation = Center[0];
	Success &= FCamerasDataParserHelper::ParseValueToFloat(InOutLines, XRadius);
	Success &= FCamerasDataParserHelper::ParseValueToFloat(InOutLines, YRadius);
	Success &= FCamerasDataParserHelper::ParseValueToFloat(InOutLines, Height);
	Success &= FCamerasDataParserHelper::ParseValueToInt32(InOutLines, Size);
	Success &= FCamerasDataParserHelper::ParseMarker(InOutLines, FCamerasDataParserHelper::EMarkerType::EndMarker);

	Success &= FCamerasDataParserHelper::ParseValueToFloat(InOutLines, Duration);
	Success &= FCamerasDataParserHelper::ParseValueToString(InOutLines, Category);

	//phases
	Success &= FCamerasDataParserHelper::ParseValueToStringArray(InOutLines, Phases);

	Success &= FCamerasDataParserHelper::ParseMarker(InOutLines, FCamerasDataParserHelper::EMarkerType::EndMarker);
	Success &= FCamerasDataParserHelper::ParseToLocations(InOutLines, Locations);
	Success &= FCamerasDataParserHelper::ParseToRotations(InOutLines, Rotations);

	Success &= FCamerasDataParserHelper::ParseMarker(InOutLines, FCamerasDataParserHelper::EMarkerType::EndMarker);
	bHasLoadedData = Success;

	bHasLoadedData = true;

	return Success;
}

FString FAnimatedCameraNodeData::ToString() const
{
/*
	FString CameraName;
	FString CameraType;
	float Duration;
	FString Category;
	TArray<FString> Phases;


	FVector CenterLocation;
	float Height;
	float XRadius;
	float YRadius;


	TArray<FVector> Locations;
	TArray<FRotator> Rotations;

	FString GetSerializedString(int32 Indent) const;
	bool SerializeDataFromString(TArray<FString> & InOutLines);

	Unserialized data 
	bool bHasLoadedData;

*/
	FString Result;
	if (bHasLoadedData)
	{
		return TEXT("Does not have loaded data");
	}
	Result.Append(FString::Printf(TEXT("{%s}"), *CameraName));
	Result.Append(FString::Printf(TEXT("{%s}"), *CameraType));
	Result.Append(FString::Printf(TEXT("{%f}"), Duration));
	Result.Append(FString::Printf(TEXT("{%s}"), *Category));
	Result.Append(FString::Printf(TEXT("{Center:%s}"), *CenterLocation.ToString()));

	return Result;

}

