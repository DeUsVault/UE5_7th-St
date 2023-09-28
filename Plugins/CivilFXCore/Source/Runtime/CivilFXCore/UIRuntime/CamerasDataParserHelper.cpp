// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CamerasDataParserHelper.h"
#include "Templates/UnrealTemplate.h"

const TCHAR* const FCamerasDataParserHelper::Tab = TEXT("\t");
const TCHAR* const FCamerasDataParserHelper::Newline = TEXT("\r\n");
const TCHAR* const FCamerasDataParserHelper::StartMarker = TEXT("{");
const TCHAR* const FCamerasDataParserHelper::EndMarker = TEXT("}");
const TCHAR* const FCamerasDataParserHelper::Seperator = TEXT(":");
const TCHAR* const FCamerasDataParserHelper::SubSeperator = TEXT(",");
const TCHAR* const FCamerasDataParserHelper::SMNL = TEXT("{\r\n");
const TCHAR* const FCamerasDataParserHelper::EMNL = TEXT("}\r\n");


bool FCamerasDataParserHelper::ShouldParse(const TArray<FString>& InOutLines)
{
	return InOutLines.Num() > 0;
}

bool FCamerasDataParserHelper::ParseHeader(TArray<FString>& InOutLines, FString& OutHeader)
{
	bool Success = ShouldParse(InOutLines);
	if (Success)
	{
		OutHeader = MoveTemp(InOutLines[0]);
		InOutLines.RemoveAt(0);
	}
	return Success;
}

bool FCamerasDataParserHelper::ParseMarker(TArray<FString>& InOutLines, EMarkerType MarkerType)
{
	const TCHAR* const Marker = MarkerType == EMarkerType::StartMarker ? StartMarker : EndMarker;
	bool Success = ShouldParse(InOutLines);
	if (Success)
	{
		FString TempString = InOutLines[0].TrimStartAndEnd();
		Success = TempString.Equals(Marker);
		if (Success)
		{
			InOutLines.RemoveAt(0);
		}
	}
	return Success;
}



bool FCamerasDataParserHelper::ParseValueToString(TArray<FString>& InOutLines, FString& OutValue)
{
	bool Success = ShouldParse(InOutLines);
	if (Success)
	{
		FString& TempString = InOutLines[0];
		TempString.TrimStartAndEndInline();

		TArray<FString> Lines;
		TempString.ParseIntoArray(Lines, Seperator);
		Success &= Lines.Num() == 2;
		if (Success)
		{
			OutValue = MoveTemp(Lines[1]);
			InOutLines.RemoveAt(0);
		}
	}
	return Success;
}


bool FCamerasDataParserHelper::ParseValueToStringArray(TArray<FString>& InOutLines, TArray<FString>& OutArray)
{
	FString Temp;
	bool bSuccess = ParseValueToString(InOutLines, Temp);
	if (bSuccess)
	{
		GLog->Log(Temp);
		Temp.ParseIntoArray(OutArray, SubSeperator);

		for (const FString& Str : OutArray)
		{
			GLog->Log(Str);
		}

	}
	return bSuccess;
}

bool FCamerasDataParserHelper::ParseValueToInt32(TArray<FString>& InOutLines, int32& OutValue)
{
	FString TempString;
	bool Success = ParseValueToString(InOutLines, TempString);
	if (Success)
	{
		OutValue = FCString::Atoi(*TempString);
	}
	return Success;
}

bool FCamerasDataParserHelper::ParseValueToFloat(TArray<FString>& InOutLines, float& OutValue)
{
	FString TempString;
	bool Success = ParseValueToString(InOutLines, TempString);
	if (Success)
	{
		OutValue = FCString::Atof(*TempString);
	}
	return Success;
}

bool FCamerasDataParserHelper::ParseToVector(TArray<FString>& InOutLines, FVector& OutValue)
{
	bool Success = ParseMarker(InOutLines, EMarkerType::StartMarker);
	if (Success)
	{
		Success &= ParseValueToFloat(InOutLines, (float&) OutValue.X);
		Success &= ParseValueToFloat(InOutLines, (float&) OutValue.Y);
		Success &= ParseValueToFloat(InOutLines, (float&) OutValue.Z);
		Success &= ParseMarker(InOutLines, EMarkerType::EndMarker);
	}
	return Success;
}


bool FCamerasDataParserHelper::ParseToLocations(TArray<FString>& InOutLines, TArray<FVector>& OutLocations)
{
	FString Header;
	OutLocations.Empty();
	bool Success = ParseHeader(InOutLines, Header);
	do
	{
		FVector Location;
		Success &= ParseToVector(InOutLines, Location);
		if (Success)
		{
			OutLocations.Add(Location);
		}
	} while (Success);

	return OutLocations.Num() > 0;
}

bool FCamerasDataParserHelper::ParseToRotations(TArray<FString>& InOutLines, TArray<FRotator>& OutRotations)
{
	TArray<FVector> Eulers;
	bool Success = ParseToLocations(InOutLines, Eulers);
	if (Success)
	{
		for (const FVector& Euler : Eulers)
		{
			OutRotations.Add(FRotator::MakeFromEuler(Euler));
		}
	}
	return OutRotations.Num() > 0;
}

void FCamerasDataParserHelper::BuildLocationString(FString& InOutString, const TArray<FVector>& InLocations, int32 Indent)
{
	int32 Count = InLocations.Num();
	if (Count > 0)
	{
		static const TCHAR* const Header = Count == 1 ? TEXT("Location") : TEXT("Locations");
		AppendTab(InOutString, Indent);
		InOutString.Append(Header);
		InOutString.Append(Newline);

		for (const FVector& Location : InLocations)
		{
			AppendTab(InOutString, Indent);
			InOutString.Append(SMNL);

			AppendTab(InOutString, Indent + 1);
			InOutString.Append(FString::Format(TEXT("X:{0}{1}"), { Location.X, Newline }));
			AppendTab(InOutString, Indent + 1);
			InOutString.Append(FString::Format(TEXT("Y:{0}{1}"), { Location.Y, Newline }));
			AppendTab(InOutString, Indent + 1);
			InOutString.Append(FString::Format(TEXT("Z:{0}{1}"), { Location.Z, Newline }));

			AppendTab(InOutString, Indent);
			InOutString.Append(EMNL);
		}
	}
}

void FCamerasDataParserHelper::BuildRotationString(FString& InOutString, const TArray<FRotator>& InRotations, int32 Indent)
{
	int32 Count = InRotations.Num();
	if (Count > 0)
	{
		static const TCHAR* const Header = Count == 1 ? TEXT("Rotation") : TEXT("Rotations");
		AppendTab(InOutString, Indent);
		InOutString.Append(Header);
		InOutString.Append(Newline);

		for (const FRotator& Rotation : InRotations)
		{
			const FVector Euler = Rotation.Euler();

			AppendTab(InOutString, Indent);
			InOutString.Append(SMNL);

			AppendTab(InOutString, Indent + 1);
			InOutString.Append(FString::Format(TEXT("X:{0}{1}"), { Euler.X, Newline }));
			AppendTab(InOutString, Indent + 1);
			InOutString.Append(FString::Format(TEXT("Y:{0}{1}"), { Euler.Y, Newline }));
			AppendTab(InOutString, Indent + 1);
			InOutString.Append(FString::Format(TEXT("Z:{0}{1}"), { Euler.Z, Newline }));

			AppendTab(InOutString, Indent);
			InOutString.Append(EMNL);
		}
	}
}

void FCamerasDataParserHelper::AppendTab(FString& InOutString, int32 TabCount)
{
	for (int32 i = 0; i < TabCount; ++i)
	{
		InOutString.Append(Tab);
	}
}

/*
FString FCamerasDataParserHelper::CombinePhases(const TArray<FString>& Phases)
{
	FString Temp;
	for (int32 i = 0; i < Phases.Num(); ++i)
	{
		Temp.Append(Phases[i]);
		if (i != Phases.Num() - 1)
		{
			Temp.Append(SubSeperator);
		}
	}
	return Temp;
}

TArray<FString> FCamerasDataParserHelper::SplitPhases(FString Phases)
{
	
}
*/