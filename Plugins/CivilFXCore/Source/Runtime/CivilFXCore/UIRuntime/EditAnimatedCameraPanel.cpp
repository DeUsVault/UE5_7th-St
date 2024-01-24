// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCameraPanel.h"

#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "CivilFXCore/UIRuntime/NavigationPanel.h"

#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"

#include "EditAnimatedCameraOverviewPanel.h"
#include "EditAnimatedCameraPropertyPanel.h"
#include "EditAnimatedCameraFocusviewPanel.h"
#include "EditAnimatedCamFocusviewMovement.h"

#include "UIRuntimeUtils.h"

#include "Slate/SceneViewport.h"

void UEditAnimatedCameraPanel::NativeConstruct()
{
	PlayerPawn = Cast<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	/** Spawn 2d capture component */
	ScenceCapture2D = GetWorld()->SpawnActor<ASceneCapture2D>(FVector::ZeroVector, FRotator::ZeroRotator);
	check(ScenceCapture2D);
	ScenceCapture2D->AttachToActor(GetWorld()->GetFirstPlayerController()->PlayerCameraManager, FAttachmentTransformRules::SnapToTargetIncludingScale);
	ScenceCapture2D->GetCaptureComponent2D()->TextureTarget = TargetTexture;

	FocusviewPanel->MovementPanel->PanelWithRenderWidgetViewport = this;

	/** Redirecting some references*/
	OverviewPanel->SetPropertyPanel(PropertyPanel);
	PropertyPanel->SetFocusviewPanel(FocusviewPanel);

	/** Binding delegates */
	AddNodeButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraPanel::HandleAddNodeButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UEditAnimatedCameraPanel::HandleExitButtonClicked);
	SearchCameraInputBox->OnTextChanged.AddDynamic(this, &UEditAnimatedCameraPanel::HandleSearchBoxTextChanged);


	/** Turn off viewport camera */
	//PlayerCameraManager->PCOwner->GetLocalPlayer()->ViewportClient->EngineShowFlags.Rendering = false;
	//GetWorld()->GetGameViewport()->GetGameViewport()->GetClient()->GetEngineShowFlags()->Rendering = false;
	//GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->ViewportClient->EngineShowFlags.Rendering = false;
}

void UEditAnimatedCameraPanel::OnPanelSelected()
{
	PlayAnimation(PanelAnim);
}

void UEditAnimatedCameraPanel::RefreshPanel(const TArray<FAnimatedCameraNodeData>& InCameraNodeDatas,
	const TArray<FCameraNodeData_ID>& InIds)
{
	TArray<FString> CategorySourceNames;
	CategorySourceNames.Add(TEXT("Default"));

	for (int32 Index = 0; Index< InCameraNodeDatas.Num(); ++Index)
	{
		const FAnimatedCameraNodeData& CameraNodeData = InCameraNodeDatas[Index];
		TSharedPtr<FAnimatedCameraNodeData> SharedCameraNodeData = MakeShared<FAnimatedCameraNodeData>(CameraNodeData);
		const int32 Id = InIds.IsValidIndex(Index) ? InIds[Index].Id : -1;
		OverviewPanel->AddChild(SharedCameraNodeData, Id);
		CategorySourceNames.AddUnique(CameraNodeData.Category);
	}

	PropertyPanel->SetComboBoxCategorySource(CategorySourceNames);
}

void UEditAnimatedCameraPanel::OnBeginRecording()
{
	Container->SetVisibility(ESlateVisibility::Collapsed);
}

void UEditAnimatedCameraPanel::OnEndRecording()
{
	Container->SetVisibility(ESlateVisibility::Visible);
}

void UEditAnimatedCameraPanel::HandleAddNodeButtonClicked()
{
	OverviewPanel->AddChild(true);
	SearchCameraInputBox->SetText(FText::GetEmpty());
}

void UEditAnimatedCameraPanel::HandleExitButtonClicked()
{
	GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->ViewportClient->EngineShowFlags.Rendering = true;
	TArray<FCameraNodeData_ID> Ids;
	TArray<FAnimatedCameraNodeData> CameraNodeDatas = OverviewPanel->GetAllCameraNodeData(Ids);
	for (FAnimatedCameraNodeData& Data : CameraNodeDatas)
	{
		Data.bHasLoadedData = true;
	}
	OnEditAnimatedCameraPanelExited.Broadcast(CameraNodeDatas, Ids);
	ScenceCapture2D->Destroy(false, false);

	/** Let other panels cleaning up too*/
	FocusviewPanel->OnExited();

	RemoveFromViewport();
}

void UEditAnimatedCameraPanel::HandleSearchBoxTextChanged(const FText& InputText)
{
	OverviewPanel->DisplaySearchedChildren(InputText);
}

