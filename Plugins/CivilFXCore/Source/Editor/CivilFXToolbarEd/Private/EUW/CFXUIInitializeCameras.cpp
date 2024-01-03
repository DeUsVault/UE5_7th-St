// Fill out your copyright notice in the Description page of Project Settings.


#include "EUW/CFXUIInitializeCameras.h"

#include "CivilFXCore/UIRuntime/NavigationPanel.h"
#include "CivilFXCore/CommonCore/CivilFXCoreSettings.h"

#include "Components/DetailsView.h"
#include "Misc/FileHelper.h"
#include "JsonObjectConverter.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

void UCFXUIInitializeCameras::NativePreConstruct()
{
	if (!ViewObject)
	{
		ViewObject = NewObject<UCFXInitializeCamerasObject>(this);
	}
	DetailsView->SetObject(ViewObject);
}

void UCFXUIInitializeCameras::HandleUploadRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		BP_OnUploadRequestCompleted(false, FString::Format(TEXT("Failed to upload cameras. Reason: {0}"), { Response->GetContentAsString() }));
		return;
	}

	BP_OnUploadRequestCompleted(true, TEXT("Success"));
}

void UCFXUIInitializeCameras::InitializeCamerasOnServer()
{
	const UCivilFXCoreSettings* CoreSettings = GetDefault<UCivilFXCoreSettings>();
	if (CoreSettings->EndPoint.IsEmpty() || CoreSettings->ResourceToken.IsEmpty())
	{
		BP_OnUploadRequestCompleted(false, TEXT("EndPoint or ResourceToken is empty"));
		return;
	}

	//Delete all cameras for this token
	TSharedRef<IHttpRequest> DeleteRequest = 
		UNavigationPanel::CreateRequest(TEXT("DELETE"), TEXT("navdata"));
	DeleteRequest->OnProcessRequestComplete().BindWeakLambda(this, [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (!bWasSuccessful || !EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			BP_OnUploadRequestCompleted(false, FString::Format(TEXT("Failed to delete all cameras. Reason: {0}"), { Response->GetContentAsString() }));
			return;
		}

		FString JsonStr;
		bool bSucess = FFileHelper::LoadFileToString(JsonStr, *ViewObject->FilePath.FilePath);
		if (!bSucess)
		{
			BP_OnUploadRequestCompleted(false, TEXT("Failed to load cameras from file"));
			return;
		}

		//Upload data
		TSharedRef<IHttpRequest> UploadRequest =
			UNavigationPanel::CreateRequest(TEXT("POST"), TEXT("navdata"));
		UploadRequest->SetContentAsString(JsonStr);
		UploadRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleUploadRequestCompleted);
		UploadRequest->ProcessRequest();
	});
	DeleteRequest->ProcessRequest();
}
