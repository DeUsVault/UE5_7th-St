// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraOverviewPanel.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "EditAnimatedCameraPropertyPanel.h"

#include "LockedDialogPanel.h"


void UEditAnimatedCameraOverviewPanel::AddChild(bool bChildSelected)
{
	AddChild(MakeShared<FAnimatedCameraNodeData>(), bChildSelected);
}

void UEditAnimatedCameraOverviewPanel::AddChild(TSharedPtr<FAnimatedCameraNodeData> CameraData, bool bChildSelected/*=false*/)
{
	UWidget* Child = CreateWidget(GetWorld(), ImageButtonClass);
	check(Child);

	Container->AddChild(Child);
	UScrollBoxSlot* ChildSlot = Cast<UScrollBoxSlot>(Child->Slot);
	ChildSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	ChildSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);


	UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);
	ChildButton->GetButton()->OnClicked.AddDynamic(this, &UEditAnimatedCameraOverviewPanel::OnEditAnimatedCameraButtonSelected);
	ChildButton->SetCameraNodeData(CameraData);
	ChildButton->ParentPanel = this;

	if (bChildSelected || (CurrentSelectedButton == nullptr && Container->GetAllChildren().Num() == 1))
	{
		ChildButton->GetButton()->OnClicked.Broadcast();
		Container->ScrollWidgetIntoView(ChildButton);
	}
}

void UEditAnimatedCameraOverviewPanel::DisplaySearchedChildren(const FText& SeachedText)
{
	//show all children first
	for (UWidget* Child : Container->GetAllChildren())
	{
		Child->SetVisibility(ESlateVisibility::Visible);
		UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);
		ChildButton->SetHighlightText(FText::GetEmpty());
	}

	FString SearchedName = SeachedText.ToString();
	//early exit
	if (SearchedName.IsEmpty())
	{
		return;
	}

	for (UWidget* Child : Container->GetAllChildren())
	{
		UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);
		if (ChildButton)
		{
			if (ChildButton->GetCameraName().ToString().Contains(SearchedName))
			{
				ChildButton->SetVisibility(ESlateVisibility::Visible);
				ChildButton->SetHighlightText(FText::FromString(SearchedName));
			}
			else
			{
				ChildButton->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UEditAnimatedCameraOverviewPanel::OnEditAnimatedCameraButtonSelected()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		UEditAnimatedCameraOverviewButton* NewSelectedButton = nullptr;
		for (UWidget* Child : Container->GetAllChildren())
		{
			UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);

			if (ChildButton->IsButtonSelected() && ChildButton != CurrentSelectedButton)
			{
				NewSelectedButton = ChildButton;
				break;
			}
		}
		if (NewSelectedButton)
		{
			if (CurrentSelectedButton)
			{
				CurrentSelectedButton->OnDeselected();
			}
			CurrentSelectedButton = NewSelectedButton;
		}

		PropertyPanel->SetEnable();
		PropertyPanel->RefreshPanel(CurrentSelectedButton);
	});
}



void UEditAnimatedCameraOverviewPanel::DeleteChild(UEditAnimatedCameraOverviewButton* DeletingButton)
{
	int32 Index = Container->GetChildIndex(CurrentSelectedButton);

	Container->RemoveChild(CurrentSelectedButton);

	if (Container->GetChildrenCount() > 0)
	{
		Index = FMath::Min(Index + 1, Container->GetChildrenCount() - 1);
		UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Container->GetChildAt(Index));
		ChildButton->GetButton()->OnClicked.Broadcast();
		Container->ScrollWidgetIntoView(ChildButton);
	}
	else
	{	//add a default child
		AddChild(true);
	}
}

void UEditAnimatedCameraOverviewPanel::SetPropertyPanel(class UEditAnimatedCameraPropertyPanel* InPropertyPanel)
{
	PropertyPanel = InPropertyPanel;
}

TArray<FAnimatedCameraNodeData> UEditAnimatedCameraOverviewPanel::GetAllCameraNodeData()
{
	TArray<FAnimatedCameraNodeData> Result;
	for (UWidget* Child : Container->GetAllChildren())
	{
		UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);
		if (ChildButton)
		{	
			Result.Add(*ChildButton->GetCameraNodeData());
		}
	}
	return Result;
}
