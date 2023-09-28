// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/UnrealType.h"

/**
 * 
 */
class CIVILFXCORE_API FUIRuntimeUtils
{
public:
//     static void BuildDefaultCategorySource(const TArray<FString>& CategoryNames);
//     static const TArray<TSharedPtr<FText>>* GetCategorySource();
//     static void AddToCategorySource(FText CategoryText);

private:
    static TArray<TSharedPtr<FText>> CategorySource;
};
