// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CFXUIInitializeCameras.generated.h"

class UDetailsView;

class IHttpRequest;
class IHttpResponse;
typedef TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> FHttpRequestPtr;
typedef TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> FHttpResponsePtr;

UCLASS()
class CIVILFXTOOLBARED_API UCFXInitializeCamerasObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FFilePath FilePath;
};

/**
 * 
 */
UCLASS()
class CIVILFXTOOLBARED_API UCFXUIInitializeCameras : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativePreConstruct() override;

	void HandleUploadRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:

	UFUNCTION(BlueprintCallable, CallInEditor)
	void InitializeCamerasOnServer();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnUploadRequestCompleted(bool bSuccess, const FString& ErrorMessage);

protected:

	UPROPERTY(meta=(BindWidget))
	UDetailsView* DetailsView;

	UPROPERTY(Transient)
	UCFXInitializeCamerasObject* ViewObject;
};
