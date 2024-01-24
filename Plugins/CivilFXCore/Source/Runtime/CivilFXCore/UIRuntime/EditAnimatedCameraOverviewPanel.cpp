// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraOverviewPanel.h"
#include "CivilFXCore/UIRuntime/NavigationPanel.h"

#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "EditAnimatedCameraPropertyPanel.h"

#include "LockedDialogPanel.h"

#include "JsonObjectConverter.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

void UEditAnimatedCameraOverviewPanel::AddChild(bool bChildSelected)
{
	if (UNavigationPanel::UseApi())
	{
		FString Content;
		FAnimatedCameraNodeData DefaultData;
		FJsonObjectConverter::UStructToJsonObjectString(DefaultData, Content);

		FHttpRequestPtr AddRequest = UNavigationPanel::CreateRequest(TEXT("POST"), TEXT("animated"));
		AddRequest->SetContentAsString(Content);
		AddRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleAddAnimatedCameraAPICompleted);
		AddRequest->ProcessRequest();
	}
	else 
	{
		AddChild(MakeShared<FAnimatedCameraNodeData>(), -1, bChildSelected);
	}
}

void UEditAnimatedCameraOverviewPanel::AddChild(TSharedPtr<FAnimatedCameraNodeData> CameraData, int32 Id, bool bChildSelected/*=false*/)
{
	UWidget* Child = CreateWidget(GetWorld(), ImageButtonClass);
	check(Child);

	Container->AddChild(Child);
	UScrollBoxSlot* ChildSlot = Cast<UScrollBoxSlot>(Child->Slot);
	ChildSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	ChildSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);


	UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);
	ChildButton->GetButton()->OnClicked.AddDynamic(this, &UEditAnimatedCameraOverviewPanel::OnEditAnimatedCameraButtonSelected);
	ChildButton->SetCameraNodeData(CameraData, Id);
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

void UEditAnimatedCameraOverviewPanel::HandleAddAnimatedCameraAPICompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		const FString& Content = Response->GetContentAsString();

		FAnimatedCameraNodeData NodeData;
		FJsonObjectConverter::JsonObjectStringToUStruct(Content, &NodeData);

		FCameraNodeData_ID Id;
		FJsonObjectConverter::JsonObjectStringToUStruct(Content, &Id);

		AddChild(MakeShared<FAnimatedCameraNodeData>(NodeData), Id.Id);
	}
}

void UEditAnimatedCameraOverviewPanel::DeleteChild(UEditAnimatedCameraOverviewButton* DeletingButton)
{
	int32 Index = Container->GetChildIndex(DeletingButton);
	Container->RemoveChild(DeletingButton);

	if (Container->GetChildrenCount() == 0)
	{
		//add a default child
		AddChild(true);
	}

	if (DeletingButton == CurrentSelectedButton)
	{
		Index = FMath::Min(Index + 1, Container->GetChildrenCount() - 1);
		if (Index >= 0)
		{
			UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Container->GetChildAt(Index));
			ChildButton->GetButton()->OnClicked.Broadcast();
			Container->ScrollWidgetIntoView(ChildButton);
		}
	}
}

void UEditAnimatedCameraOverviewPanel::SetPropertyPanel(class UEditAnimatedCameraPropertyPanel* InPropertyPanel)
{
	PropertyPanel = InPropertyPanel;
}

TArray<FAnimatedCameraNodeData> UEditAnimatedCameraOverviewPanel::GetAllCameraNodeData(TArray<FCameraNodeData_ID>& OutIds) const
{
	OutIds.Empty();

	TArray<FAnimatedCameraNodeData> Result;
	for (UWidget* Child : Container->GetAllChildren())
	{
		UEditAnimatedCameraOverviewButton* ChildButton = Cast<UEditAnimatedCameraOverviewButton>(Child);
		if (ChildButton)
		{	
			Result.Add(*ChildButton->GetCameraNodeData());
			OutIds.Add( { ChildButton->GetCameraNodeId() } );
		}
	}
	return Result;
}