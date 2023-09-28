// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditCameraFocusviewExportVideoSP.h"
#include "RenderingPanel.h"
#include "TextableButton.h"
#include "EditAnimatedCameraFocusviewPanel.h"
#include "EditAnimatedCameraPanel.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UObject/UObjectIterator.h"
#include "Components/Button.h"

#include "EditAnimatedCameraOverviewButton.h"

void UEditCameraFocusviewExportVideoSP::NativeConstruct()
{
	Super::NativeConstruct();

	BackButton->OnClicked.AddDynamic(this, &UEditCameraFocusviewExportVideoSP::HandleBackButtonClicked);

	Video4KButton->GetButton()->OnClicked.AddDynamic(this, &UEditCameraFocusviewExportVideoSP::Handle4KButtonClicked);
	Video1080PUltraButton->GetButton()->OnClicked.AddDynamic(this, &UEditCameraFocusviewExportVideoSP::Handle1080PUltraButtonClick);
	Video1080PButton->GetButton()->OnClicked.AddDynamic(this, &UEditCameraFocusviewExportVideoSP::Handle1080PButtonClicked);


	/** get reference to the RenderingPanel */
	/* For some reason GetAllWidgetsOfClass() returns empty */
	/* So we manually iterate*/
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* LiveWidget = *Itr;
		if (LiveWidget->GetClass()->IsChildOf(URenderingPanel::StaticClass()) && GetWorld() == LiveWidget->GetWorld())
		{
			RenderingPanel = Cast<URenderingPanel>(LiveWidget);
			break;
		}
	}
	check(RenderingPanel);
	
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UEditAnimatedCameraPanel::StaticClass());
	OuterEditPanel = Cast<UEditAnimatedCameraPanel>(AllWidgets[0]);
	check(OuterEditPanel);
}

void UEditCameraFocusviewExportVideoSP::OnPanelVisible()
{
	ParentPanel->OnCameraPreviewEnded.AddDynamic(this, &UEditCameraFocusviewExportVideoSP::HandleCameraPreviewEnded);
}

void UEditCameraFocusviewExportVideoSP::OnPanelHidden()
{
	ParentPanel->OnCameraPreviewEnded.RemoveAll(this);
}

void UEditCameraFocusviewExportVideoSP::HandleBackButtonClicked()
{
	ParentPanel->ShowPreviewControlPanel();
	EditingButton->GetButton()->OnClicked.Broadcast();
}

void UEditCameraFocusviewExportVideoSP::Handle4KButtonClicked()
{
	ParentPanel->OnBeginRecording();
	OuterEditPanel->OnBeginRecording();
	RenderingPanel->Video4KButton->GetButton()->OnClicked.Broadcast();

	Video4KButton->OnDeselected();
}

void UEditCameraFocusviewExportVideoSP::Handle1080PUltraButtonClick()
{
	ParentPanel->OnBeginRecording();
	OuterEditPanel->OnBeginRecording();
	RenderingPanel->Video1080PUltraButton->GetButton()->OnClicked.Broadcast();

	Video1080PUltraButton->OnDeselected();
}

void UEditCameraFocusviewExportVideoSP::Handle1080PButtonClicked()
{
	ParentPanel->OnBeginRecording();
	OuterEditPanel->OnBeginRecording();
	RenderingPanel->Video1080PButton->GetButton()->OnClicked.Broadcast();

	Video1080PButton->OnDeselected();
}

void UEditCameraFocusviewExportVideoSP::HandleCameraPreviewEnded()
{
	OuterEditPanel->OnEndRecording();
	RenderingPanel->StopRecordingButton->GetButton()->OnClicked.Broadcast();
}
