// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "NavigationPanel.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/ExpandableArea.h"
#include "Components/VerticalBox.h"

#include "DialogPanel.h"
#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "CamerasDataParserHelper.h"
#include "TextableButton.h"
#include "AnimatedCameraButtonBlock.h"
#include "Navigation/StillCameraView.h"
#include "Navigation/AnimatedCameraView.h"
#include "ComboBoxCategory.h"
#include "CivilFXCore/Subsystems/SceneManagerCFX.h"

#include "Engine/Engine.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"

#include "JsonObjectConverter.h"

void UNavigationPanel::SetReferenceToHamburgerButton(UButton* HamburgerButtonRef)
{
	HamburgerButton = HamburgerButtonRef;
}

void UNavigationPanel::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPawn = Cast<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	/**Load some blueprints*/
	EditAnimatedCameraClass = LoadClass<UEditAnimatedCameraPanel>(nullptr, TEXT("WidgetBlueprint'/CivilFXCore/UI_BPs/BP_EditAnimatedCameraPanel.BP_EditAnimatedCameraPanel_C'"));

	/**Create folder and camera config file*/
	FString LaunchDir;
#if WITH_EDITOR
	LaunchDir = FPaths::ProjectDir();
#else
	LaunchDir = FPaths::LaunchDir();
#endif
	DataFolderDir = FPaths::Combine(LaunchDir, FString("Data"));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectory(*DataFolderDir);

	CameraConfigFileDir = FPaths::Combine(DataFolderDir, FString("CameraConfig.json"));

	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *CameraConfigFileDir);
	FNavigationCameraData NavData;
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &NavData, 0, 0);

	//Draw animated cameras
	CameraNodeDatas = NavData.AnimatedCamera;
	RefreshAnimatedCameraContainer();

	//Draw still cameras
	for (const FCameraViewInfo& StillInfo : NavData.StillCameras)
	{
		AddNewStillCameraToScrollBox(FText::FromString(StillInfo.CameraName), FText::FromString(StillInfo.CameraCategory), StillInfo.Rotation, StillInfo.Location);
	}

	/** Bind delegates*/
	EditAnimatedCameraButton->OnClicked.AddDynamic(this, &UNavigationPanel::HandleEditAnimatedCameraButtonClicked);
	AddStillCameraButton->OnClicked.AddDynamic(this, &UNavigationPanel::HandleAddStillCameraButtonClicked);
	GetGameInstance()->GetSubsystem<UPhaseManager>()->OnSwichPhase().AddUObject(this, &UNavigationPanel::HandleNewPhaseSwitch);
}

void UNavigationPanel::NativeDestruct()
{
	/**/
	FNavigationCameraData NavData;
	//Animated Cameras
	NavData.AnimatedCamera = CameraNodeDatas;

	//Still Cameras
	NavData.StillCameras = StillCameraView->GetCameraViews();

	//Write
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(NavData, JsonString);
	FFileHelper::SaveStringToFile(JsonString, *FPaths::Combine(DataFolderDir, FString("CameraConfig.json")));
	//
	Super::NativeDestruct();
}

void UNavigationPanel::RefreshAnimatedCameraContainer()
{
	//Filter active cameras
	TArray<FAnimatedCameraNodeData> ActiveCameras;

	EPhaseType CurrentPhase = GetGameInstance()->GetSubsystem<UPhaseManager>()->GetCurrentPhase();
	for (const FAnimatedCameraNodeData& Data : CameraNodeDatas)
	{
		bool bMatched = Data.Phases.ContainsByPredicate([CurrentPhase](const FString& PhaseTypeName)
		{
			FText TextValue = UEnum::GetDisplayValueAsText(CurrentPhase);

			#if WITH_EDITOR
			FString ValueName = TextValue.ToString();
			#else
			FString ValueName = FName::NameToDisplayString(TextValue.ToString(), false);
			#endif

			return ValueName.Equals(PhaseTypeName);
		});

		if (bMatched)
		{
			ActiveCameras.Add(Data);
		}
	}

	AnimatedCameraView->RedrawView(ActiveCameras);
}

void UNavigationPanel::HandleEditAnimatedCameraButtonClicked()
{
	//Stop any active chevron
	AnimatedCameraView->StopAllChevron();

	HamburgerButton->OnClicked.Broadcast();
	UEditAnimatedCameraPanel* EditWidget = Cast<UEditAnimatedCameraPanel>(CreateWidget(GetWorld(), EditAnimatedCameraClass));
	EditWidget->AddToViewport();
	EditWidget->OnPanelSelected();
	EditWidget->RefreshPanel(CameraNodeDatas);
	EditWidget->OnEditAnimatedCameraPanelExited.AddUObject(this, &UNavigationPanel::HandleEditAnimatedCameraPanelExited);
}

void UNavigationPanel::HandleAddStillCameraButtonClicked()
{
	TWeakObjectPtr<UDialogPanel> DialogPanel = UDialogPanel::CreateAndAddToViewport(GetWorld(), TEXT("Add Still Camera"), TEXT("Enter Camera Name..."));

	UWidget* CustomWidget = DialogPanel->AddCustomWidget(UComboBoxCategory::StaticClass());
	UComboBoxCategory* StillCameraComboBoxWidget = CastChecked<UComboBoxCategory>(CustomWidget);

	const TArray<FCameraViewInfo>& AllViews = StillCameraView->GetCameraViews();
	TArray<FString> CategorySource;
	CategorySource.Reserve(AllViews.Num() + 1);
	CategorySource.Add(TEXT("Default"));
	for (const FCameraViewInfo& View : AllViews)
	{
		CategorySource.AddUnique(View.CameraCategory);
	}

	//Add RootsName, to account for just recently emptied root
	for (const FString& RootName : StillCameraView->GetRootsName())
	{
		CategorySource.AddUnique(RootName);
	}

	if (CurrentEditingStillCameraCategoryText.IsEmpty())
	{
		CurrentEditingStillCameraCategoryText = FText::FromString(CategorySource[0]);
	}

	StillCameraComboBoxWidget->SetCategorySource(CategorySource);
	StillCameraComboBoxWidget->OnTextCommitted.BindUObject(this, &UNavigationPanel::HandleStillComboBoxCategoryTextCommitted);
	StillCameraComboBoxWidget->OnGetText.BindUObject(this, &UNavigationPanel::HandleStillComboBoxCategoryGetText);

	DialogPanel->OnConfirmButtonClicked().BindUObject(this, &UNavigationPanel::AddNewStillCameraToScrollBoxDelegate);
}

void UNavigationPanel::AddNewStillCameraToScrollBoxDelegate(FText CameraName)
{
	AddNewStillCameraToScrollBox(CameraName, CurrentEditingStillCameraCategoryText, PlayerPawn->GetActorRotation(), PlayerPawn->GetActorLocation());
}

void UNavigationPanel::AddNewStillCameraToScrollBox(const FText& CameraName, const FText& CameraCategory, const FRotator& Rotation, const FVector& Location)
{
	FCameraViewInfo View;
	View.CameraName = CameraName.ToString();
	View.CameraCategory = CameraCategory.ToString();
	View.Location = Location;
	View.Rotation = Rotation;
	View.FoV = GetWorld()->GetSubsystem<USceneManagerCFX>()->GetSceneFOV();
	StillCameraView->AddItemData(View);
}

FText UNavigationPanel::HandleStillComboBoxCategoryGetText()
{
	return CurrentEditingStillCameraCategoryText;
}

void UNavigationPanel::HandleStillComboBoxCategoryTextCommitted(const FText& InText)
{
	CurrentEditingStillCameraCategoryText = InText;
}

void UNavigationPanel::HandleEditAnimatedCameraPanelExited(TArray<FAnimatedCameraNodeData>& InOutCameraNodeDatas)
{
	HamburgerButton->OnClicked.Broadcast();
	CameraNodeDatas = MoveTemp(InOutCameraNodeDatas);
	RefreshAnimatedCameraContainer();
}

void UNavigationPanel::HandleNewPhaseSwitch(EPhaseType, EPhaseMode)
{
	RefreshAnimatedCameraContainer();
}