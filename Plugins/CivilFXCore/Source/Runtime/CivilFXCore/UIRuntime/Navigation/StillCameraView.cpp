// Fill out your copyright notice in the Description page of Project Settings.


#include "StillCameraView.h"

#include "Components/TreeView.h"
#include "CivilFXCore/UIRuntime/NavigationPanel.h"

void UStillCameraView::NativeConstruct()
{
	TreeViewContainer->SetOnGetItemChildren(this, &UStillCameraView::HandleGetItemChildren);
}

void UStillCameraView::AddItemData(const FCameraViewInfo& InView, int32 Id)
{
	//Check if should create a new root
	int32 RootIndex = Roots.IndexOfByPredicate([CameraCategory=InView.CameraCategory](UCameraHierarchyRoot* Root)
	{
		return Root->IsSame(CameraCategory);
	});

	UCameraHierarchyRoot* CurrentRoot = nullptr;
	if (RootIndex != INDEX_NONE)
	{
		CurrentRoot = Roots[RootIndex];
	} else 
	{
		CurrentRoot = NewObject<UCameraHierarchyRoot>();
		CurrentRoot->SetRootText(InView.CameraCategory);
		TreeViewContainer->SetItemExpansion(CurrentRoot, true);
		Roots.Add(CurrentRoot);
	}

	UStillCameraHierarchyWidget* Object = UStillCameraHierarchyWidget::MakeObject(InView, Id);
	CurrentRoot->AddChild(Object);

	//Do this later to avoid an extra UTreeView::RequestRefresh();
	if (RootIndex == INDEX_NONE)
	{
		TreeViewContainer->AddItem(CurrentRoot);
	}
	else
	{
		TreeViewContainer->RequestRefresh();
	}
}

TArray<FCameraViewInfo> UStillCameraView::GetCameraViews() const
{
	TArray<FCameraViewInfo> Views;

	for (UCameraHierarchyRoot* Root : Roots)
	{
		TArray<UStillCameraHierarchyWidget*> Children;
		Root->GetAllChildren(Children);
		for (UStillCameraHierarchyWidget* Child : Children)
		{
			/**
			 * [11/10/2021]
			 * Fix up category as result of reordering items via drag and drop.
			 *	Note: This is a lazy fix, the category should be updated as soon as the reordering happens.
			 *		But since nothing else is using it, it's fine to do it here.
			 */
			Child->CameraView.CameraCategory = Root->GetText().ToString();
			//~
			Views.Add(Child->CameraView);
		}
	}

	return Views;
}

TArray<FString> UStillCameraView::GetRootsName() const
{
	TArray<FString> RootsName;
	for (UCameraHierarchyRoot* Root : Roots)
	{
		RootsName.Add(Root->GetText().ToString());
	}
	return RootsName;
}

void UStillCameraView::RequestRemoveItem(UCameraHierarchyModel* Model)
{
	for (UCameraHierarchyRoot* Root : Roots)
	{
		if (Root->IsParentOf(Model))
		{
			Root->RemoveChild(Model);
			TreeViewContainer->RequestRefresh();

			//Remove from database
			if (UStillCameraHierarchyWidget* StillModel = Cast<UStillCameraHierarchyWidget>(Model))
			{
				UNavigationPanel::RemoveStillCamera(StillModel->Id);
			}
		}
	}
}

void UStillCameraView::SetIsEditDragDrop(bool bInIsEditDragDrop)
{
	bIsEditDragDrop = bInIsEditDragDrop;
	TreeViewContainer->ClearSelection();
	TreeViewContainer->RegenerateAllEntries();
}

void UStillCameraView::HandleGetItemChildren(UObject* Item, TArray<UObject*>& OutChildren)
{
	UCameraHierarchyModel* CameraModel = CastChecked<UCameraHierarchyModel>(Item);
	CameraModel->GetAllChildren(OutChildren);
}
