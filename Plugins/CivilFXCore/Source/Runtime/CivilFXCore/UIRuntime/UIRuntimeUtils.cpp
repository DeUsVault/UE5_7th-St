// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "UIRuntimeUtils.h"

TArray<TSharedPtr<FText>> FUIRuntimeUtils::CategorySource;

// void FUIRuntimeUtils::BuildDefaultCategorySource(const TArray<FString>& CategoryNames)
// {
// 	CategorySource.Reset();
// 	CategorySource.Add(MakeShareable(new FText(FText::FromString(TEXT("Default")))));
// 
// 	for (const FString& Name : CategoryNames)
// 	{
// 		AddToCategorySource(FText::FromString(Name));
// 	}
// 
// 	GLog->Log(FString::Printf(TEXT("BuildDefaultCategorySource: %d"), CategorySource.Num()));
// }
// 
// const TArray<TSharedPtr<FText>>* FUIRuntimeUtils::GetCategorySource()
// {
// 	return &CategorySource;
// }
// 
// void FUIRuntimeUtils::AddToCategorySource(FText CategoryText)
// {
// 	bool bNewCategory = true;
// 	for (int32 j = 0; j < CategorySource.Num() && bNewCategory; ++j)
// 	{
// 		bNewCategory &= !CategorySource[j].Get()->EqualTo(CategoryText);
// 	}
// 	if (bNewCategory)
// 	{
// 		CategorySource.Add(MakeShareable(new FText(CategoryText)));
// 	}
// }
