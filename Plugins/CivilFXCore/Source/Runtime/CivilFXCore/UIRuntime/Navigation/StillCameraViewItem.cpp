// Fill out your copyright notice in the Description page of Project Settings.


#include "StillCameraViewItem.h"

#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "StillCameraView.h"
#include "CivilFXCore/UIRuntime/Components/DragDropDecorator.h"


#include "Styling/CoreStyle.h"
#include "Components/TreeView.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

//==============


//==============
void UCameraHierarchyModel::AddChild(UCameraHierarchyModel* Model)
{
	Children.AddUnique(Model);
	Model->Parent = this;
}

void UCameraHierarchyModel::InsertChild(UCameraHierarchyModel* InModel, UCameraHierarchyModel* InModelAfter /*= nullptr*/)
{
	if (!InModelAfter)
	{
		AddChild(InModel);
	}
	else
	{
		int32 Index = Children.IndexOfByKey(InModelAfter);
		check(Index != INDEX_NONE);

		InModel->Parent = this;
		Children.Insert(InModel, Index + 1);
	}
}

void UCameraHierarchyModel::RemoveChild(UCameraHierarchyModel* Model)
{
	Children.Remove(Model);
	Model->Parent = nullptr;
}

void UCameraHierarchyModel::MoveChildToIndex(UCameraHierarchyModel* InModel, int32 InIndex)
{
	check(Children.Contains(InModel));
	Children.Remove(InModel);
	Children.Insert(InModel, InIndex);
}

UCameraHierarchyModel* UCameraHierarchyModel::GetParent() const
{
	return Parent;
}

bool UCameraHierarchyModel::IsParentOf(UCameraHierarchyModel* Model) const
{
	return Children.Contains(Model);
}

void UCameraHierarchyModel::ReorderChildren(UCameraHierarchyModel* DraggedChild, UCameraHierarchyModel* DroppedChild)
{
	check(Children.Contains(DraggedChild));
	check(Children.Contains(DroppedChild));

	Children.Remove(DraggedChild);
	int32 Index = Children.IndexOfByKey(DroppedChild);
	check(Index != INDEX_NONE);
	Children.Insert(DraggedChild, Index + 1);
}

void UCameraHierarchyModel::SetOuterOwner(UObject* InOwner)
{
	OuterOwner = InOwner;
}

UObject* UCameraHierarchyModel::GetOuterOwner() const
{
	if (OuterOwner.IsValid())
	{
		return OuterOwner.Get();
	}
	return nullptr;
}

TOptional<EItemDropZone> UCameraHierarchyModel::HandleCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, UCameraHierarchyModel* TargetItem)
{
	if (TargetItem == this)
	{
		return TOptional<EItemDropZone>();
	}

	UObject* ThisOwner = GetOuterOwner();
	UObject* TargetOwner = TargetItem->GetOuterOwner();

	if (ThisOwner != nullptr && ThisOwner == TargetOwner)
	{
		return EItemDropZone::OntoItem;
	}

	return TOptional<EItemDropZone>();
}

//=============


UStillCameraHierarchyWidget* UStillCameraHierarchyWidget::MakeObject(const FCameraViewInfo& InView)
{
	UStillCameraHierarchyWidget* Object = NewObject<UStillCameraHierarchyWidget>();
	Object->CameraView = InView;

	return Object;
}

//==============

void UStillCameraViewItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UE_LOG(LogTemp, Warning, TEXT("UStillCameraViewItem::NativeOnListItemObjectSet"));

	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	bool bIsEditDragDrog = false;
	if (UStillCameraView* OwnerView = GetOuterOwnerView())
	{
		bIsEditDragDrog = OwnerView->GetIsEditDragDrop();
	}
	Model = CastChecked<UCameraHierarchyModel>(ListItemObject);
	Model->SetOuterOwner(GetOuterOwnerView());
	BP_RefreshViewItem(Model->GetText(), Model->IsRoot(), bIsEditDragDrog & !Model->IsRoot() /*Not supported dragdrop root*/);
}

void UStillCameraViewItem::NativeOnItemSelectionChanged(bool bIsSelected)
{
	UE_LOG(LogTemp, Warning, TEXT("UStillCameraViewItem::NativeOnItemSelectionChanged"));

	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	bool bIsEditDragDrop = false;
	if (UStillCameraView* OwnerView = GetOuterOwnerView())
	{
		bIsEditDragDrop = OwnerView->GetIsEditDragDrop();
	}
	if (bIsSelected && !bIsEditDragDrop)
	{
		if (UStillCameraHierarchyWidget* ViewModel = Cast<UStillCameraHierarchyWidget>(Model))
		{
			ACivilFXPawn* PlayerPawn = CastChecked<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
			PlayerPawn->OnBeginCameraMovement(ViewModel->CameraView.Location, ViewModel->CameraView.Rotation, 3.0f);
		}
	}
}

bool UStillCameraViewItem::IsRoot() const
{
	return Model->IsRoot();
}
PRAGMA_DISABLE_OPTIMIZATION
void UStillCameraViewItem::HandleDeleteButtonClicked()
{
	check(!IsRoot());

	UTreeView* OwningView = CastChecked<UTreeView>(GetOwningListView());
	UStillCameraView* ViewContainer = CastChecked<UStillCameraView>(OwningView->GetOuter()->GetOuter());
	ViewContainer->RequestRemoveItem(Model);
}
PRAGMA_ENABLE_OPTIMIZATION
void UStillCameraViewItem::HandleExpandButtonClicked()
{
	check(Model->IsRoot());

	const bool bExpanded = IsListItemExpanded();
	UTreeView* OwningView = CastChecked<UTreeView>(GetOwningListView());
	OwningView->SetItemExpansion(Model, !bExpanded);
}

int32 UStillCameraViewItem::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (ItemDropZone.IsSet())
	{
		// Draw feedback for user dropping an item above, below, or onto a row.
		const FSlateBrush* DropIndicatorBrush = [&]()
		{
			const FTableRowStyle* Style = &FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");

			switch (ItemDropZone.GetValue())
			{
			case EItemDropZone::AboveItem: return &Style->DropIndicator_Above; break;
			default:
			case EItemDropZone::OntoItem: return &Style->DropIndicator_Onto; break;
			case EItemDropZone::BelowItem: return &Style->DropIndicator_Below; break;
			};
		}();

		FSlateDrawElement::MakeBox
		(
			OutDrawElements,
			LayerId++,
			AllottedGeometry.ToPaintGeometry(),
			DropIndicatorBrush,
			ESlateDrawEffect::None,
			FColor::Cyan
		);
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UStillCameraViewItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("UStillCameraViewItem::NativeOnDragDetected"));
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	OutOperation = NewObject<UDragDropOperation>(this);
	OutOperation->Payload = Model;
	OutOperation->Pivot = EDragPivot::CenterLeft;
	{
		UDragDropDecorator* Decorator = WidgetTree->ConstructWidget<UDragDropDecorator>();
		Decorator->SetBorderBrushColor(FLinearColor::Black);
		Decorator->SetText(FText::FromString(TEXT("Drag detected")));

		OutOperation->DefaultDragVisual = Decorator;
	}
}

void UStillCameraViewItem::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UDragDropDecorator* Decorator = CastChecked<UDragDropDecorator>(InOperation->DefaultDragVisual);
	const bool bIsRoot = IsRoot();
	const FText Text = FText::FromString(TEXT("Place row below here"));
	Decorator->SetText(Text);
}

void UStillCameraViewItem::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	ItemDropZone = TOptional<EItemDropZone>();
}

bool UStillCameraViewItem::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	ItemDropZone = Model->HandleCanAcceptDrop(InDragDropEvent, EItemDropZone::OntoItem, CastChecked<UCameraHierarchyModel>(InOperation->Payload));
	return ItemDropZone.IsSet();
}

bool UStillCameraViewItem::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//Reset the indicator
	ItemDropZone = TOptional<EItemDropZone>();

	//
	UCameraHierarchyModel* DraggedItem = CastChecked<UCameraHierarchyModel>(InOperation->Payload);
	if (IsRoot())
	{
		if (!Model->IsParentOf(DraggedItem))
		{
			UCameraHierarchyModel* DraggedItemParent = DraggedItem->GetParent();
			DraggedItemParent->RemoveChild(DraggedItem);

			Model->AddChild(DraggedItem);
		}
		else
		{
			Model->MoveChildToIndex(DraggedItem, 0);
		}
		UTreeView* TreeOwner = CastChecked<UTreeView>(GetOwningListView());
		TreeOwner->SetItemExpansion(Model, true);
	}
	else
	{
		UCameraHierarchyModel* DraggedItemParent = DraggedItem->GetParent();
		UCameraHierarchyModel* DroggedItemParent = Model->GetParent();
		if (DraggedItemParent == DroggedItemParent)
		{
			//Just reorder
			DroggedItemParent->ReorderChildren(DraggedItem, Model);
		}
		else
		{
			DraggedItemParent->RemoveChild(DraggedItem);
			DroggedItemParent->InsertChild(DraggedItem, Model);
		}
	}

	GetOwningListView()->RequestRefresh();
	return true;
}

void UStillCameraViewItem::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("UStillCameraViewItem::NativeOnDragCancelled"));

	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

}

void UStillCameraViewItem::HandleMainButtonClicked()
{
	if (Model->IsRoot())
	{
		HandleExpandButtonClicked();
	}
	else
	{
		//Not root
		UTreeView* OwnerView = CastChecked<UTreeView>(GetOwningListView());
		OwnerView->SetSelectedItem(Model);
	}
}

UStillCameraView* UStillCameraViewItem::GetOuterOwnerView() const
{
	UTreeView* OwningView = CastChecked<UTreeView>(GetOwningListView());
	UStillCameraView* ViewContainer = Cast<UStillCameraView>(OwningView->GetOuter()->GetOuter());
	return ViewContainer;
}
