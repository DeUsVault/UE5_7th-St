// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraOverviewButton.h"
#include "EditAnimatedCameraOverviewPanel.h"
#include "EditOverviewButtonOptionsMenu.h"
#include "LockedDialogPanel.h"
#include "CivilFXCore/UIRuntime/NavigationPanel.h"

#include "Components/TextBlock.h"
#include "Components/MenuAnchor.h"

#include "Widgets/Text/STextBlock.h"


void UEditAnimatedCameraOverviewButton::NativeConstruct()
{
	Super::NativeConstruct();

	MoreOptionsButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraOverviewButton::HandleMoreOptionsButtonClicked);
	

	AnchorMenuContent = CreateWidget(GetWorld(), MoreOptionsMenuAnchor->MenuClass);
	UEditOverviewButtonOptionsMenu* MenuContent = Cast<UEditOverviewButtonOptionsMenu>(AnchorMenuContent);
	check(MenuContent);

	MenuContent->GetDeleteButton()->OnClicked.AddDynamic(this, &UEditAnimatedCameraOverviewButton::HandleDeleteButtonClicked);

	MoreOptionsMenuAnchor->OnGetMenuContentEvent.BindDynamic(this, &UEditAnimatedCameraOverviewButton::HandleMenuAnchorGetContent);
}

void UEditAnimatedCameraOverviewButton::SetCameraName(const FText& NewName)
{
	CameraNameTextBox->SetText(NewName);
}

FText UEditAnimatedCameraOverviewButton::GetCameraName() const
{
	return CameraNameTextBox->GetText();
}

void UEditAnimatedCameraOverviewButton::SetHighlightText(FText InText)
{
	TSharedPtr<STextBlock> TextSlate = StaticCastSharedPtr<STextBlock>(CameraNameTextBox->GetCachedWidget());
	TextSlate->SetHighlightText(InText);
}

void UEditAnimatedCameraOverviewButton::UpdateCameraNodeData()
{

}

void UEditAnimatedCameraOverviewButton::SetCameraNodeData(TSharedPtr<FAnimatedCameraNodeData> InCameraData, int32 InId)
{
	CameraNodeData = InCameraData;
	Id = InId;

	SetCameraName(FText::FromString(CameraNodeData->CameraName));

	if (CameraNodeData->bHasLoadedData)
	{
		FVector ThumbnailLocation = CameraNodeData->Locations[0];
		if (CameraNodeData->IsMovementType())
		{
			ThumbnailLocation.Z += CameraNodeData->Height;
		}
		GenerateThumbnail(ThumbnailLocation, CameraNodeData->Rotations[0]);
	}
}

TSharedPtr<FAnimatedCameraNodeData> UEditAnimatedCameraOverviewButton::GetCameraNodeData() const
{
	return CameraNodeData;
}

int32 UEditAnimatedCameraOverviewButton::GetCameraNodeId() const
{
	return Id;
}

void UEditAnimatedCameraOverviewButton::HandleDeleteButtonClicked()
{
	GLog->Log("Deleting");
	bPendingDelete = true;

	TWeakObjectPtr<ULockedDialogPanel> DialogPanel = ULockedDialogPanel::CreateAndAddToViewport(GetWorld(), TEXT("Delete Camera"));
	FString DialogContent = FString::Printf(TEXT("Delete \t%s?"), *GetCameraName().ToString());
	DialogPanel->SetContent(DialogContent);
	DialogPanel->OnConfirmButtonClicked().BindUObject(this, &UEditAnimatedCameraOverviewButton::HandleConfirmedDelete);
	DialogPanel->OnCancelButtonClicked().BindUObject(this, &UEditAnimatedCameraOverviewButton::HandleCanceledDelete);
}

void UEditAnimatedCameraOverviewButton::HandleConfirmedDelete(FText)
{
	//let parent panel delete this button
	ParentPanel->DeleteChild(this);

	//Delete from database
	UNavigationPanel::RemoveAnimatedCamera(Id);
}

void UEditAnimatedCameraOverviewButton::HandleCanceledDelete()
{
	bPendingDelete = false;
}

void UEditAnimatedCameraOverviewButton::HandleMoreOptionsButtonClicked()
{
	MoreOptionsMenuAnchor->ToggleOpen(true);
}

UWidget* UEditAnimatedCameraOverviewButton::HandleMenuAnchorGetContent()
{
	return AnchorMenuContent;
}

