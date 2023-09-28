// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CFXMediaCapture.h"
#include "CFXMediaOutput.h"

#include "fccore.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Slate/SceneViewport.h"
#include "Engine/Engine.h"
#include "Engine/GameEngine.h"
#include "Async/Async.h"

#include "VideoEncoderThread.h"

#include "ImageResampling/Base/vnImage.h"
#include "ImageResampling/vnImagine.h"


void UCFXMediaCapture::StartCapture(class UTextureRenderTarget2D* TargetTexture, const FString& FilePath, FIntPoint Resolution, int32 BitRate, ECFXMediaCaptureFrameRate FrameRate)
{
	TargetFrameRate = FrameRate == ECFXMediaCaptureFrameRate::SIXTY ? 60 : 30;

	int32 VideoWidth = Resolution.X;
	int32 VideoHeight = Resolution.Y;
	bEnableSampling = false;

	UCFXMediaOutput* Output = Cast<UCFXMediaOutput>(MediaOutput);
	if (Output->SamplingOptions.bUseSampling)
	{
		bEnableSampling = true;
		SampledSize = Output->SamplingOptions.SampledSize;

		VideoWidth = SampledSize.X;
		VideoHeight = SampledSize.Y;

		SourceImage = MakeShared<CVImage>();
		vnCreateImage(VN_IMAGE_FORMAT_R16G16B16A16F, Output->SamplingOptions.SourceSize.X, Output->SamplingOptions.SourceSize.Y, SourceImage.Get());
	}


	fcMP4Config conf;
	conf.video = true;
	conf.video_width = VideoWidth;
	conf.video_height = VideoHeight;
	conf.video_bitrate_mode = fcBitrateMode::VBR;
	conf.video_target_bitrate = BitRate;
	conf.video_target_framerate = TargetFrameRate;
	conf.video_max_tasks = 12;
	conf.audio = false;

	RecordedFrames = 0;
	RecordedDuration = 0;

	RecorderContext = fcMP4OSCreateContext(&conf, TCHAR_TO_ANSI(*FilePath));

	FMediaCaptureOptions Options;
	Options.bResizeSourceBuffer = true;

	//CaptureActiveSceneViewport(Options);
	CaptureTextureRenderTarget2D(TargetTexture, Options);

	EncoderThread = new FVideoEncoderThread;
	EncoderThread->Create(TEXT("ET"), RecorderContext, TPri_Highest);


	OnRecordingDurationExceededWrapper = [DynamicCB = RecordingDurationExceededTask.OnRecordingDurationExceeded]()
	{
		DynamicCB.ExecuteIfBound();
	};

	/*
	OnCompleteWrapper = [NativeCB = FileMediaOutput->WriteOptions.NativeOnComplete, DynamicCB = FileMediaOutput->WriteOptions.OnComplete](bool bSuccess)
	{
		if (NativeCB)
		{
			NativeCB(bSuccess);
		}
		DynamicCB.ExecuteIfBound(bSuccess);
	};
	*/

	//CaptureActiveSceneViewport(Options);
	//check(MediaOutput);
	//check(MediaOutput->GetWorld());
	//UWorld* World = MediaOutput->GetWorld();
	//check(GetWorld());
	//check(GetWorld()->GetGameViewport());
	//check(GetWorld()->GetGameViewport()->GetGameViewport());

	/*
	GLog->Log(MediaOutput->GetWorld()->GetGameViewport()->GetGameViewport()->GetSize().ToString());
	GLog->Log("Setting fixed size");
	MediaOutput->GetWorld()->GetGameViewport()->GetGameViewport()->SetFixedViewportSize(1920, 1080);
	GLog->Log(MediaOutput->GetWorld()->GetGameViewport()->GetGameViewport()->GetSize().ToString());
	*/
	//FViewportClient c;
	//FSceneViewport* SV = MediaOutput->GetWorld()->GetGameViewport()->GetGameViewport();

	/*
	UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
	check(GameEngine);

	GameEngine->SceneViewport->SetFixedViewportSize(1280, 720);
	TSharedPtr<FSceneViewport> SceneViewport = GameEngine->SceneViewport;

	CaptureSceneViewport(SceneViewport, FMediaCaptureOptions());
	*/

	/*
	ASceneCapture2D* SceneCapture2D = World->SpawnActor<ASceneCapture2D>(FVector::ZeroVector, FRotator::ZeroRotator);
	SceneCapture2D->AttachToActor(World->GetFirstPlayerController()->PlayerCameraManager, FAttachmentTransformRules::SnapToTargetIncludingScale);

	UTextureRenderTarget2D* TargetTexture = UKismetRenderingLibrary::CreateRenderTarget2D(World, 150, 100, RTF_RGBA16f);
	check(TargetTexture);
	SceneCapture2D->GetCaptureComponent2D()->TextureTarget = TargetTexture;

	FMediaCaptureOptions Options;
	Options.bResizeSourceBuffer = true;
	CaptureTextureRenderTarget2D(TargetTexture, Options);
	*/

	/*
	return;

	UWorld* World = MediaOutput->GetWorld();
	check(World);
	TSharedPtr<SViewport> ViewportWidget = World->GetGameViewport()->GetGameViewportWidget();
	//FViewportClient* ViewportClient = dynamic_cast<FViewportClient*>(MediaOutput->GetWorld()->GetGameViewport());

	UGameViewportClient* GameViewportClient = World->GetGameViewport();
	//TSharedPtr<FSceneViewport> SceneViewport = MakeShareable(new FSceneViewport(ViewportClient, ViewportWidget));
	TSharedPtr<FSceneViewport> SceneViewport = MakeShareable(new FSceneViewport(GameViewportClient, ViewportWidget));

	check(SceneViewport.IsValid());
	GLog->Log("Testing");
	GLog->Log(SceneViewport->GetSizeXY().ToString());

	FMediaCaptureOptions Options;
	Options.bResizeSourceBuffer = false;
	//CaptureActiveSceneViewport(Options);

	CaptureSceneViewport(SceneViewport, Options);
	*/
}

void UCFXMediaCapture::OnFrameCaptured_RenderingThread(const FCaptureBaseData& InBaseData, TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData, void* InBuffer, int32 Width, int32 Height, int32 BytesPerRow)
{
	//CVImage SourceImage;

	static int32 Index = 0;
	
	//const double TimeElapsed = InBaseData.SourceFrameTimecode.ToTimespan(InBaseData.SourceFrameTimecodeFramerate).GetTotalSeconds() - BaseTimestamp;
	//GLog->Log(FString::Printf(TEXT("%f"), TimeElapsed));

	//GLog->Log(InBaseData.SourceFrameTimecodeFramerate);

	void* ImagePtr = InBuffer;

	CVImage ResampledImage;
	if (bEnableSampling)
	{
		FMemory::Memcpy(SourceImage->QueryData(), InBuffer, SourceImage->SlicePitch());
		vnResizeImage(*SourceImage.Get(), VN_IMAGE_KERNEL_LANCZOS, SampledSize.X, SampledSize.Y, 0, &ResampledImage);
		ImagePtr = reinterpret_cast<void*>(ResampledImage.QueryData());
	}

	//GLog->Log(FString::Printf(TEXT("FramesCounter: %d"), EncoderThread->FramesCounter.Load(EMemoryOrder::Relaxed)));

	//int32 EncoderThreadFramesInQueue = EncoderThread->FramesCounter.Load(EMemoryOrder::Relaxed);
	//GLog->Log(FString::Printf(TEXT("FramesCounter: %d"), EncoderThreadFramesInQueue));

	const double LTimeStamp = 1.0 / TargetFrameRate * RecordedFrames;
	RecordedDuration = LTimeStamp;

	if (false)
	//if (EncoderThreadFramesInQueue < 5)
	{
		TUniquePtr<FVideoFrameTask> VideoTask = MakeUnique<FVideoFrameTask>();
		TUniquePtr<TImagePixelData<FFloat16Color>> PixelData = MakeUnique<TImagePixelData<FFloat16Color>>(FIntPoint(Width, Height));
		PixelData->Pixels = TArray<FFloat16Color>(reinterpret_cast<FFloat16Color*>(InBuffer), Width * Height);
		VideoTask->PixelData = MoveTemp(PixelData);
		VideoTask->Timestamp = LTimeStamp;

		EncoderThread->EnqueueWork(MoveTemp(VideoTask));
	}
	else
	{
		//GLog->Log(FString::Printf(TEXT("Timestamp from rendering: %f"), LTimeStamp));
		fcMP4AddVideoFramePixels(RecorderContext, ImagePtr, fcPixelFormat::fcPixelFormat_RGBAf16, 0);
	}
	++RecordedFrames;

	if (RecordingDurationExceededTask.bHasTask && LTimeStamp >= RecordingDurationExceededTask.Duration)
	{
		TFunction<void()> Wrapper = OnRecordingDurationExceededWrapper;
		AsyncTask(ENamedThreads::GameThread, [Wrapper]
		{
			Wrapper();
		});
		RecordingDurationExceededTask.bHasTask = false;
	}

	//VN_STATUS Status;

	//Status = vnCreateImage(VN_IMAGE_FORMAT_R16G16B16A16F, Width, Height, &SourceImage);
	//GLog->Log(FString::Printf(TEXT("Create Source Image: %d - %d"), Status, SourceImage.QueryWidth()));


	//Status = vnCreateImage(VN_IMAGE_FORMAT_R16G16B16A16F, 1920, 1080, &ResampledImage);
	//GLog->Log(FString::Printf(TEXT("Create ResampledImage Image: %d"), Status));



	//Status = vnResizeImage(SourceImage, VN_IMAGE_KERNEL_LANCZOS2, 1920, 1080, 0, &ResampledImage);

	//GLog->Log(FString::Printf(TEXT("vnLanczosKernel: %d"), Status));


	//GLog->Log(FString::Printf(TEXT("OnFrameCaptured_RenderingThread")));
	//GLog->Log(MediaOutput->GetWorld()->GetGameViewport()->GetGameViewport()->GetRenderTargetTextureSizeXY().ToString());
	//GLog->Log(FString::Printf(TEXT("%s"), *InBaseData.SourceFrameTimecodeFramerate.ToPrettyText().ToString()));
	//GLog->Log(FString::Printf(TEXT("%f"), InBaseData.SourceFrameTimecodeFramerate.AsDecimal()));
	/*
	TUniquePtr<TImagePixelData<FFloat16Color>> PixelData = MakeUnique<TImagePixelData<FFloat16Color>>(FIntPoint(Width, Height));
	PixelData->Pixels = TArray<FFloat16Color>(reinterpret_cast<FFloat16Color*>(InBuffer), Width * Height);
	ImageTask->PixelData = MoveTemp(PixelData);
	*/
	/*
	const float LTimeStamp = 1.0 / TargetFrameRate * RecordedFrames;
	fcMP4AddVideoFramePixels(RecorderContext, ImagePtr, fcPixelFormat::fcPixelFormat_RGBAf16, LTimeStamp);
	++RecordedFrames;
	*/
	vnDestroyImage(&ResampledImage);

	//	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_ChannelMask, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_TypeMask, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Type_f16, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Type_f32, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Type_u8, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Type_i16, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Type_i32, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Rf16, TimeStamp); //**
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGf16, TimeStamp); //**
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBf16, TimeStamp); //C
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBAf16, TimeStamp); //**
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Rf32, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGf32, TimeStamp); //C
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBf32, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBAf32, TimeStamp); //C
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Ru8, TimeStamp); //**
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGu8, TimeStamp); //**
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBu8, TimeStamp); //**
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBAu8, TimeStamp); //**
	//	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Ri16, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGi16, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBi16, TimeStamp); //C
	//	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBAi16, TimeStamp); //C
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_Ri32, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGi32, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBi32, TimeStamp); //C
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_RGBAi32, TimeStamp); //C
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_I420, TimeStamp);
	// 	fcMP4AddVideoFramePixels(RecorderContext, InBuffer, fcPixelFormat::fcPixelFormat_NV12, TimeStamp);

}

bool UCFXMediaCapture::InitializeCapture()
{
	return true;
}

bool UCFXMediaCapture::PostInitializeCaptureViewport(TSharedPtr<FSceneViewport>& InSceneViewport)
{
	GLog->Log("CaptureSceneViewportImpl");
	SetState(EMediaCaptureState::Capturing);
	return true;
}

bool UCFXMediaCapture::PostInitializeCaptureRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
	GLog->Log("CaptureRenderTargetImpl");

	BaseTimecode = FApp::GetTimecode();
	BaseTimecodeFramerate = FApp::GetTimecodeFrameRate();
	BaseTimestamp = BaseTimecode.ToTimespan(BaseTimecodeFramerate).GetTotalSeconds();

	SetState(EMediaCaptureState::Capturing);
	return true;
}

void UCFXMediaCapture::StopCaptureImpl(bool bAllowPendingFrameToBeProcess)
{
	EncoderThread->KillThread();
	delete EncoderThread;

	GLog->Log("StopCaptureImpl");
	fcReleaseContext(RecorderContext);
	RecorderContext = nullptr;
	vnDestroyImage(SourceImage.Get());
	SourceImage = nullptr;
}
