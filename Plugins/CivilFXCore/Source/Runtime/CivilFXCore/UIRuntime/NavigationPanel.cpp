// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "NavigationPanel.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/ExpandableArea.h"
#include "Components/VerticalBox.h"

#include "DialogPanel.h"
#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "CivilFXCore/CommonCore/CivilFXCoreSettings.h"
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

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

void UNavigationPanel::SetReferenceToHamburgerButton(UButton* HamburgerButtonRef)
{
	HamburgerButton = HamburgerButtonRef;
}

bool UNavigationPanel::UseApi()
{
	const UCivilFXCoreSettings* CoreSettings = GetDefault<UCivilFXCoreSettings>();

	return CoreSettings->bUseAPI 
		&& !CoreSettings->EndPoint.IsEmpty()
		&& !CoreSettings->ResourceToken.IsEmpty();
}

TSharedRef<IHttpRequest> UNavigationPanel::CreateRequest(const FString& InVerb, const FString& InRoute, const TOptional<int32>& InId /*={}*/)
{
	const UCivilFXCoreSettings* CoreSettings = GetDefault<UCivilFXCoreSettings>();
	FString EndPoint = CoreSettings->EndPoint;
	FPaths::NormalizeDirectoryName(EndPoint);
	const FString ResourceToken = CoreSettings->ResourceToken;

	FString Url = FString::Format(TEXT("{0}/{1}/{2}"), { EndPoint, InRoute, ResourceToken });
	if (InId.IsSet())
	{
		Url = FString::Format(TEXT("{0}/{1}"), { Url, InId.GetValue() });
	}
	
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(InVerb);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetURL(Url);

	return HttpRequest;
}

void UNavigationPanel::RemoveStillCamera(int32 Id)
{
	TSharedRef<IHttpRequest> DeleteRequest = CreateRequest(TEXT("DELETE"), TEXT("still"), Id);
	DeleteRequest->ProcessRequest();
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

	const UCivilFXCoreSettings* Settings = GetDefault<UCivilFXCoreSettings>();
	if (UseApi())
	{
		//Load cameras from API
		TSharedRef<IHttpRequest> HttpRequest = CreateRequest(TEXT("GET"), TEXT("navdata"));
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleCamerasAPICompleted);
		HttpRequest->ProcessRequest();
	}
	else 
	{
		//Load cameras from disk
		FString JsonString;
		FFileHelper::LoadFileToString(JsonString, *CameraConfigFileDir);
		UpdatePanelCameras(JsonString);
	}

	/** Bind delegates*/
	EditAnimatedCameraButton->OnClicked.AddDynamic(this, &UNavigationPanel::HandleEditAnimatedCameraButtonClicked);
	AddStillCameraButton->OnClicked.AddDynamic(this, &UNavigationPanel::HandleAddStillCameraButtonClicked);
	GetGameInstance()->GetSubsystem<UPhaseManager>()->OnSwitchPhase().AddUObject(this, &UNavigationPanel::HandleNewPhaseSwitch);
}

void UNavigationPanel::NativeDestruct()
{
	if (!UseApi())
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
	}

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
	if (UseApi())
	{
		FCameraViewInfo View;
		View.CameraName = CameraName.ToString();
		View.CameraCategory = CurrentEditingStillCameraCategoryText.ToString();
		View.Location = PlayerPawn->GetActorLocation();
		View.Rotation = PlayerPawn->GetActorRotation();
		View.FoV = GetWorld()->GetSubsystem<USceneManagerCFX>()->GetSceneFOV();

		FString Content;
		FJsonObjectConverter::UStructToJsonObjectString(View, Content);

		FHttpRequestRef AddStillRequest = CreateRequest(TEXT("POST"), TEXT("still"));
		AddStillRequest->SetContentAsString(Content);
		AddStillRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleAddStillCameraAPICompleted);
		AddStillRequest->ProcessRequest();
	}
	else 
	{
		AddNewStillCameraToScrollBox(CameraName, CurrentEditingStillCameraCategoryText, PlayerPawn->GetActorRotation(), PlayerPawn->GetActorLocation(), -1);
	}
}

void UNavigationPanel::AddNewStillCameraToScrollBox(const FText& CameraName, const FText& CameraCategory, const FRotator& Rotation, const FVector& Location, int32 Id)
{
	FCameraViewInfo View;
	View.CameraName = CameraName.ToString();
	View.CameraCategory = CameraCategory.ToString();
	View.Location = Location;
	View.Rotation = Rotation;
	View.FoV = GetWorld()->GetSubsystem<USceneManagerCFX>()->GetSceneFOV();
	StillCameraView->AddItemData(View, Id);
}

FText UNavigationPanel::HandleStillComboBoxCategoryGetText()
{
	return CurrentEditingStillCameraCategoryText;
}

void UNavigationPanel::HandleStillComboBoxCategoryTextCommitted(const FText& InText)
{
	CurrentEditingStillCameraCategoryText = InText;
}

void UNavigationPanel::UpdatePanelCameras(const FString& InJsonString)
{
	FNavigationCameraData NavData;
	FJsonObjectConverter::JsonObjectStringToUStruct(InJsonString, &NavData, 0, 0);

	FNavigationCameraData_ID NavData_ID;
	FJsonObjectConverter::JsonObjectStringToUStruct(InJsonString, &NavData_ID, 0, 0);

	UpdatePanelCameras(NavData, NavData_ID);
}

void UNavigationPanel::UpdatePanelCameras(const FNavigationCameraData& InNavData, const FNavigationCameraData_ID& InNavIds)
{
	//Draw animated cameras
	CameraNodeDatas = InNavData.AnimatedCamera;
	RefreshAnimatedCameraContainer();

	//Draw still cameras
	for (int32 Index = 0; Index < InNavData.StillCameras.Num(); ++Index)
	{
		const FCameraViewInfo& StillInfo = InNavData.StillCameras[Index];
		const int32 Id = InNavIds.StillCameras.IsValidIndex(Index) ? InNavIds.StillCameras[Index].Id : -1;
		AddNewStillCameraToScrollBox(FText::FromString(StillInfo.CameraName), FText::FromString(StillInfo.CameraCategory), StillInfo.Rotation, StillInfo.Location, Id);
	}
}

void UNavigationPanel::HandleCamerasAPICompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		const FString Json = Response->GetContentAsString();
		UpdatePanelCameras(Json);
	}
}

void UNavigationPanel::HandleAddStillCameraAPICompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		const FString& Content = Response->GetContentAsString();

		FCameraViewInfo View;
		FJsonObjectConverter::JsonObjectStringToUStruct(Content, &View);

		FCameraNodeData_ID Id;
		FJsonObjectConverter::JsonObjectStringToUStruct(Content, &Id);

		AddNewStillCameraToScrollBox(FText::FromString(View.CameraName), FText::FromString(View.CameraCategory), View.Rotation, View.Location, Id.Id);
	}
}

void UNavigationPanel::HandleAddAnimatedCameraAPICompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
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