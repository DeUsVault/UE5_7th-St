// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct CIVILFXCORE_API FCamerasDataParserHelper
{
    enum class EMarkerType : uint8
    {
        StartMarker,
        EndMarker
    };

    static const TCHAR* const Tab;
    static const TCHAR* const Newline;
    static const TCHAR* const StartMarker;
    static const TCHAR* const EndMarker;
    static const TCHAR* const Seperator;
    static const TCHAR* const SubSeperator;
    static const TCHAR* const SMNL; //StartMarker and Newline
    static const TCHAR* const EMNL; //EndMarker and Newline

    static bool ShouldParse(const TArray<FString>& InOutLines);
    static bool ParseHeader(TArray<FString>& InOutLines, FString& OutHeader);
    static bool ParseMarker(TArray<FString>& InOutLines, EMarkerType MarkerType);
    
    static bool ParseValueToInt32(TArray<FString>& InOutLines, int32& OutValue);
    static bool ParseValueToFloat(TArray<FString>& InOutLines, float& OutValue);
    static bool ParseValueToString(TArray<FString>& InOutLines, FString& OutValue);
    static bool ParseValueToStringArray(TArray<FString>& InOutLines, TArray<FString>& OutArray);

    /**
     * Parse a whole block (location) into FVector value, including header and markers
     */
    static bool ParseToVector(TArray<FString>& InOutLines, FVector& OutValue);

    /**
    * Parse a whole block (Rotation) into FVector value, including header and markers
    */


    static bool ParseToLocations(TArray<FString>& InOutLines, TArray<FVector>& OutLocations);
    static bool ParseToRotations(TArray<FString>& InOutLines, TArray<FRotator>& OutRotations);

    static void BuildLocationString(FString& OutString, const TArray<FVector>& InLocations, int32 Indent);
    static void BuildRotationString(FString& OutString, const TArray<FRotator>& InRotations, int32 Indent);


    static void AppendTab(FString& InOutString, int32 TabCount);


};


