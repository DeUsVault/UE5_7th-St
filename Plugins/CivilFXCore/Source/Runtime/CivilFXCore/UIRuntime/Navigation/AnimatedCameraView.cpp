// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatedCameraView.h"
#include "AnimatedCameraViewItem.h"
#include "../AnimatedCameraNodeData.h"

#include "Components/TreeView.h"

void UAnimatedCameraView::NativeConstruct()
{
	TreeViewContainer->SetOnGetItemChildren(this, &UAnimatedCameraView::HandleGetItemChildren);
}

void UAnimatedCameraView::StopAllChevron()
{
	for (UAnimatedCameraHierarchyWidget* Item : CachedItems)
	{
		Item->StopChevron();
	}
}

void UAnimatedCameraView::RedrawView(const TArray<FAnimatedCameraNodeData>& InCameraNodeDatas)
{
	if (InCameraNodeDatas.Num() == 0)
	{
		TreeViewContainer->ClearListItems();
		return;
	}

	CachedItems.Reset();

	TArray<UCameraHierarchyRoot*> Roots;

	//Build roots and items
	TMultiMap<FString, FAnimatedCameraNodeData> MultiMap;
	for (const FAnimatedCameraNodeData& Data : InCameraNodeDatas)
	{
		MultiMap.Add(Data.Category, Data);
	}
	TArray<FString> Keys;
	MultiMap.GetKeys(Keys);
	Keys.Sort();
	for (const FString& RootCategory : Keys)
	{
		UCameraHierarchyRoot* Root = NewObject<UCameraHierarchyRoot>();
		Root->SetRootText(RootCategory);
		Roots.Add(Root);

		TArray<FAnimatedCameraNodeData> AnimatedCameraData;
		MultiMap.MultiFind(RootCategory, AnimatedCameraData, false);
		AnimatedCameraData.Sort([](const FAnimatedCameraNodeData& A, const FAnimatedCameraNodeData& B) { return A.CameraName < B.CameraName; });
		for (const FAnimatedCameraNodeData& CameraData : AnimatedCameraData)
		{
			UAnimatedCameraHierarchyWidget* Item = UAnimatedCameraHierarchyWidget::MakeObject(GetWorld(), CameraData);
			Root->AddChild(Item);
		}
	}

	TreeViewContainer->SetListItems(Roots);
}

void UAnimatedCameraView::HandleGetItemChildren(UObject* Item, TArray<UObject*>& OutChildren)
{
	UCameraHierarchyModel* CameraModel = CastChecked<UCameraHierarchyModel>(Item);
	CameraModel->GetAllChildren(OutChildren);
}
