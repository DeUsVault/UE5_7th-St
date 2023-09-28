// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "RenderingPanel.h"

#include "TextableButton.h"
#include "VideoExportAdvSettingsPanel.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Components/ExpandableArea.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"

#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManagerGeneric.h"

#include "FrameCapturer/CFXMediaOutput.h"
#include "FrameCapturer/CFXMediaCapture.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Engine/Engine.h"
#include "Engine/GameEngine.h"

#include "FileMediaCapture.h"
#include "FileMediaOutput.h"

static const ECFXMediaCaptureFrameRate FrameRate = ECFXMediaCaptureFrameRate::SIXTY;

void URenderingPanel::NativeConstruct()
{
	/**Create folder and camera config file*/
	FString LaunchDir;
#if WITH_EDITOR
	LaunchDir = FPaths::ProjectDir();
#else
	LaunchDir = FPaths::LaunchDir();
#endif
	DataFolderDir = FPaths::Combine(LaunchDir, FString("Captures"));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectory(*DataFolderDir);

	VideoMediaOutput = NewObject<UCFXMediaOutput>(this, TEXT("CivilFX_MediaOutput"), RF_Transient);
	VideoMediaCapture = Cast<UCFXMediaCapture>(VideoMediaOutput->CreateMediaCapture());

	ScreenshotMediaOutput = NewObject<UFileMediaOutput>(this, TEXT("CivilFX_ScreenshotMediaOutput"), RF_Transient);
	ScreenshotMediaCapture = Cast<UFileMediaCapture>(ScreenshotMediaOutput->CreateMediaCapture());

	VideoTextureRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 150, 100, RTF_RGBA32f, FLinearColor::Black);
	//VideoTextureRenderTarget->InitCustomFormat(640, 480, PF_B8G8R8A8, true);
#if WITH_EDITOR
	VideoTextureRenderTarget->Rename(TEXT("CivilFX_Recorder_TextureTarget"));
#endif
	VideoTextureRenderTarget->TargetGamma = 2.2f; //https://answers.unrealengine.com/questions/816690/view.html


	ScreenShotTextureRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 150, 100, RTF_RGBA32f, FLinearColor::Black);
	//ScreenShotTextureRenderTarget->InitCustomFormat(640, 480, PF_B8G8R8A8, true);
	ScreenShotTextureRenderTarget->TargetGamma = 2.2f;


	bCachedUseFixedFramerate = GEngine->bUseFixedFrameRate;
	CachedFixedFramerate = GEngine->FixedFrameRate;

	Video4KButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleVideo4KButtonClicked);
	Video1080PUltraButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleVideo1080PUltraButtonClicked);
	Video1080PButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleVideoCurrentButtonClicked);
	StopRecordingButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleStopRecordingButtonClicked);
	StopRecordingButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);

	VideoAdvSettingsPanel->OnExpandableAreaExpansionChanged().AddDynamic(this, &URenderingPanel::HandleAdvSettingsAreaExpansionChanged);

	Image8KButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleImage8KButtonClicked);
	Image4KButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleImage4KButtonClicked);
	Image1080PButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::HandleImage1080PButtonClicked);
	//ImageWithUIButton->GetButton()->OnClicked.AddDynamic(this, &URenderingPanel::OnImageWithUIButtonClicked);

	//Increase texture poolsize
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	PC->ConsoleCommand(TEXT("r.Streaming.PoolSize 10000"));
}

void URenderingPanel::NativeDestruct()
{
	GEngine->bUseFixedFrameRate = bCachedUseFixedFramerate;
	GEngine->FixedFrameRate = CachedFixedFramerate;
}

void URenderingPanel::HandleVideo4KButtonClicked()
{
	Video1080PUltraButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);
	Video1080PButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);

	StopRecordingButton->GetButton()->SetVisibility(ESlateVisibility::Visible);

	CaptureVideo(FIntPoint(3840, 2160), FIntPoint(0, 0), 1000000000);
}

void URenderingPanel::HandleVideo1080PUltraButtonClicked()
{
	Video4KButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);
	Video1080PButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);

	StopRecordingButton->GetButton()->SetVisibility(ESlateVisibility::Visible);

	CaptureVideo(FIntPoint(3840, 2160), FIntPoint(1920, 1080), 1000000000);
}

void URenderingPanel::HandleVideoCurrentButtonClicked()
{
	Video1080PUltraButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);
	Video4KButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);
	StopRecordingButton->GetButton()->SetVisibility(ESlateVisibility::Visible);

	CaptureVideo(FIntPoint(1920, 1080), FIntPoint(0, 0), 1000000000);
}

void URenderingPanel::HandleStopRecordingButtonClicked()
{
	Video4KButton->GetButton()->SetVisibility(ESlateVisibility::Visible);
	DeselectButton(Video4KButton);
	Video1080PUltraButton->GetButton()->SetVisibility(ESlateVisibility::Visible);
	DeselectButton(Video1080PUltraButton);
	Video1080PButton->GetButton()->SetVisibility(ESlateVisibility::Visible);
	DeselectButton(Video1080PButton);

	StopRecordingButton->GetButton()->SetVisibility(ESlateVisibility::Hidden);
	DeselectButton(StopRecordingButton);

	VideoMediaCapture->StopCapture(false);
	SceneCapture2D->Destroy(false, false);
	SceneCapture2D = nullptr;

	FApp::SetUseFixedTimeStep(false);
	GEngine->bUseFixedFrameRate = bCachedUseFixedFramerate;
	GEngine->FixedFrameRate = CachedFixedFramerate;


	RecordedInformationText->SetVisibility(ESlateVisibility::Hidden);
	RecordedInformationText->SetText(FText::GetEmpty());
	GetWorld()->GetTimerManager().ClearTimer(UpdateInformationTimerHandle);

	//Open the folder
	FPlatformProcess::ExploreFolder(*CachedExploreFileDir);
}

void URenderingPanel::CaptureVideo(FIntPoint SourceSize, FIntPoint SampledSize, int32 BitRate, bool bWithUI/*=false*/)
{
	//GEngine->bUseFixedFrameRate = true;
	//GEngine->FixedFrameRate = FrameRate == ECFXMediaCaptureFrameRate::SIXTY ? 60 : 30;

	FFrameRate FR(FrameRate == ECFXMediaCaptureFrameRate::SIXTY ? 60 : 30, 1);
	FApp::SetFixedDeltaTime(FR.AsInterval());
	FApp::SetUseFixedTimeStep(true);
	GLog->Log(FString::Printf(TEXT("Interval %f"), FR.AsInterval()));
	
	const FString FileName = FString::Printf(TEXT("Video_%s.mp4"), *FDateTime::Now().ToString(TEXT("%Y_%m_%d-%H_%M_%S")));
	const FString FilePath = FPaths::Combine(DataFolderDir, FileName);
	CachedExploreFileDir = FilePath;

	if (!SceneCapture2D)
	{
		UMaterial* TAAClapPostProcess = LoadObject<UMaterial>(nullptr, TEXT("Material'/CivilFXCore/Materials/CapturePostProcess/M_TAAClamp.M_TAAClamp'"));
		check(TAAClapPostProcess);
		SceneCapture2D = GetWorld()->SpawnActor<ASceneCapture2D>(FVector::ZeroVector, FRotator::ZeroRotator);
		SceneCapture2D->AttachToActor(GetWorld()->GetFirstPlayerController()->PlayerCameraManager, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SceneCapture2D->GetCaptureComponent2D()->TextureTarget = VideoTextureRenderTarget;
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetPostProcessing(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetAntiAliasing(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetTemporalAA(true);
		SceneCapture2D->GetCaptureComponent2D()->PostProcessSettings.AddBlendable(TAAClapPostProcess, 1.0f);
		SceneCapture2D->GetCaptureComponent2D()->bUseRayTracingIfEnabled = true;
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetLumenDetailTraces(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetLumenReuseShadowMaps(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetRayTracedDistanceFieldShadows(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetCapsuleShadows(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetInstancedFoliage(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetInstancedStaticMeshes(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetNaniteMeshes(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetVirtualTexturePrimitives(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetVirtualShadowMapCaching(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetDynamicShadows(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetModeWidgets(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetWidgetComponents(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetDistanceCulledPrimitives(true);
		//SceneCapture2D->GetCaptureComponent2D()->PostProcessSettings
		//SceneCapture2D->Rename(TEXT("CivilFX_Recorder_SceneCapture2D"));
#if WITH_EDITOR
		SceneCapture2D->SetActorLabel(TEXT("CivilFX_Recorder_SceneCapture2D"), false);
#endif
		SceneCapture2D->GetCaptureComponent2D()->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetTonemapper(false);
	}

	GLog->Log(FString::Printf(TEXT("Size: %d, %d"), sizeof(FColor), sizeof(FFloat16Color)));


	float StopDuration = 0.f;
	bool bAutoStop = VideoAdvSettingsPanel->GetExceededRecordingDuration(StopDuration);

	VideoMediaCapture->RecordingDurationExceededTask.bHasTask = bAutoStop;
	VideoMediaCapture->RecordingDurationExceededTask.Duration = StopDuration;
	if (bAutoStop)
	{
		VideoMediaCapture->RecordingDurationExceededTask.OnRecordingDurationExceeded.BindDynamic(this, &URenderingPanel::HandleStopRecordingButtonClicked);
	}

	if (SampledSize.X > 0 && SampledSize.Y > 0)
	{
		//do sampling
		VideoMediaOutput->SamplingOptions.bUseSampling = true;
		VideoMediaOutput->SamplingOptions.SourceSize = SourceSize;
		VideoMediaOutput->SamplingOptions.SampledSize = SampledSize;
		VideoMediaCapture->StartCapture(VideoTextureRenderTarget, FilePath, FIntPoint(0,0), BitRate, FrameRate);
	}
	else
	{
		VideoMediaOutput->SamplingOptions.bUseSampling = false;
		VideoMediaOutput->DesiredSize = SourceSize;
		VideoMediaCapture->StartCapture(VideoTextureRenderTarget, FilePath, SourceSize, BitRate, FrameRate);
	}

	RecordedInformationText->SetVisibility(ESlateVisibility::Visible);
	RecordedInformationText->SetOpacity(bAutoStop ? 0.3f : 1.0f);
	GetWorld()->GetTimerManager().SetTimer(UpdateInformationTimerHandle, this, &URenderingPanel::UpdateRecordedInformation, 0.1f, true);
}


void URenderingPanel::UpdateRecordedInformation()
{
	//GLog->Log("UpdateRecordedInformation");
	float Duration = VideoMediaCapture->RecordedDuration;
	FText Text = FText::FromString(FString::Printf(TEXT("Duration: %.2f"), Duration));
	RecordedInformationText->SetText(Text);
}

void URenderingPanel::HandleAdvSettingsAreaExpansionChanged(class UExpandableArea*, bool bIsExpanded)
{
	UVerticalBoxSlot* VideoAdvSettingsPanelSlot = Cast<UVerticalBoxSlot>(VideoAdvSettingsPanel->GetParent()->Slot);
	FSlateChildSize NewSize;

	if (bIsExpanded)
	{
		ExportImageBox->SetVisibility(ESlateVisibility::Collapsed);

		NewSize.Value = 0.55f;
		NewSize.SizeRule = ESlateSizeRule::Fill;
		VideoAdvSettingsPanelSlot->SetSize(NewSize);
	}
	else
	{
		NewSize.Value = 0.1f;
		NewSize.SizeRule = ESlateSizeRule::Fill;
		VideoAdvSettingsPanelSlot->SetSize(NewSize);

		ExportImageBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void URenderingPanel::HandleImage8KButtonClicked()
{
	CaptureScreenshot(7680, 4320);
	DeselectButton(Image8KButton);
}

void URenderingPanel::HandleImage4KButtonClicked()
{
	CaptureScreenshot(3840, 2160);
	DeselectButton(Image4KButton);
}

void URenderingPanel::HandleImage1080PButtonClicked()
{
	CaptureScreenshot(1920, 1080);
	DeselectButton(Image1080PButton);
}

void URenderingPanel::OnImageWithUIButtonClicked()
{
	//CaptureScreenshot(0, 0, true);
	//DeselectButton(ImageWithUIButton);
}




void URenderingPanel::CaptureScreenshot(int32 SizeX, int32 SizeY, bool bWithUI/*=false*/)
{
	const FString FileName = FString::Printf(TEXT("Screenshot_%s.jpg"), *FDateTime::Now().ToString(TEXT("%Y_%m_%d-%H_%M_%S")));
	const FString FilePath = FPaths::Combine(DataFolderDir, FileName);
	CachedExploreFileDir = FilePath;

	if (!SceneCapture2D)
	{
		//UMaterial* TAAClapPostProcess = LoadObject<UMaterial>(nullptr, TEXT("Material'/CivilFXCore/Materials/CapturePostProcess/M_TAAClamp.M_TAAClamp'"));
		//check(TAAClapPostProcess);
		SceneCapture2D = GetWorld()->SpawnActor<ASceneCapture2D>(FVector::ZeroVector, FRotator::ZeroRotator);
		SceneCapture2D->AttachToActor(GetWorld()->GetFirstPlayerController()->PlayerCameraManager, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SceneCapture2D->GetCaptureComponent2D()->TextureTarget = ScreenShotTextureRenderTarget;
		SceneCapture2D->GetCaptureComponent2D()->bUseRayTracingIfEnabled = true;
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetPostProcessing(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetAntiAliasing(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetTemporalAA(true);
		//SceneCapture2D->GetCaptureComponent2D()->PostProcessSettings.AddBlendable(TAAClapPostProcess, 1.0f);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetLumenDetailTraces(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetLumenReuseShadowMaps(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetRayTracedDistanceFieldShadows(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetCapsuleShadows(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetInstancedFoliage(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetInstancedStaticMeshes(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetNaniteMeshes(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetVirtualTexturePrimitives(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetVirtualShadowMapCaching(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetDynamicShadows(true);
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetModeWidgets(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetWidgetComponents(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetDistanceCulledPrimitives(true);
		//SceneCapture2D->Rename(TEXT("CivilFX_Recorder_SceneCapture2D"));
#if WITH_EDITOR
		SceneCapture2D->SetActorLabel(TEXT("CivilFX_Screenshot_SceneCapture2D"), false);
#endif
		SceneCapture2D->GetCaptureComponent2D()->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		//SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetTonemapper(false);
		SceneCapture2D->GetCaptureComponent2D()->bCaptureEveryFrame = true;
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.SetScreenPercentage(true);
		SceneCapture2D->GetCaptureComponent2D()->ShowFlags.ScreenPercentage = 200;
		SceneCapture2D->GetCaptureComponent2D()->CaptureScene();
	}

	FIntPoint Resolution(SizeX, SizeY);

	ScreenshotMediaOutput->bOverrideDesiredSize = true;
	ScreenshotMediaOutput->DesiredSize = Resolution;
	//ScreenshotMediaOutput->bOverridePixelFormat = true;
	//ScreenshotMediaOutput->DesiredPixelFormat = EFileMediaOutputPixelFormat::FloatRGBA;
	ScreenshotMediaOutput->BaseFileName = FileName;
	FDirectoryPath DataFolderPath;
	DataFolderPath.Path = DataFolderDir;
	ScreenshotMediaOutput->FilePath = DataFolderPath;



	ScreenshotMediaOutput->WriteOptions.bAsync = false;
	ScreenshotMediaOutput->WriteOptions.CompressionQuality = 100;
	ScreenshotMediaOutput->WriteOptions.Format = EDesiredImageFormat::JPG;
	ScreenshotMediaOutput->WriteOptions.OnComplete.BindDynamic(this, &URenderingPanel::HandleCompleteScreenshot);

	FMediaCaptureOptions Options;
	Options.bResizeSourceBuffer = true;
	Options.bAutostopOnCapture = true;
	Options.NumberOfFramesToCapture = 1;
	ScreenshotMediaCapture->CaptureTextureRenderTarget2D(ScreenShotTextureRenderTarget, Options);
}

void URenderingPanel::HandleCompleteScreenshot(bool bSuccess)
{
	GLog->Log("Finished Screenshot");
	ScreenshotMediaCapture->StopCapture(false);

	if (SceneCapture2D)
	{
		SceneCapture2D->Destroy(false, false);
		SceneCapture2D = nullptr;
	}

	//Open the folder
	FPlatformProcess::ExploreFolder(*DataFolderDir);
}

void URenderingPanel::DeselectButton(UTextableButton* Button)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, Button]
	{
		Button->OnDeselected();
	});
}
