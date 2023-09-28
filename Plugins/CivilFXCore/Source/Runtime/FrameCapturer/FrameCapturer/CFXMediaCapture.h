// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "MediaCapture.h"
#include "CFXMediaCapture.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnRecordingDurationExceeded);


UENUM()
enum class ECFXMediaCaptureFrameRate : uint8
{
	THIRTY,
	SIXTY
};

struct FRecordingDurationExceededTask
{
	bool bHasTask;
	float Duration;
	FOnRecordingDurationExceeded OnRecordingDurationExceeded;
};

/**
 * 
 */
 UCLASS()
class FRAMECAPTURER_API UCFXMediaCapture : public UMediaCapture
{

	GENERATED_BODY()
public:
	FRecordingDurationExceededTask RecordingDurationExceededTask;
	
	void StartCapture(class UTextureRenderTarget2D* TargetTexture, const FString& FilePath, FIntPoint Resolution, int32 BitRate, ECFXMediaCaptureFrameRate FrameRate=ECFXMediaCaptureFrameRate::SIXTY);

	TAtomic<float> RecordedDuration;

protected:
	virtual bool InitializeCapture() override;
	virtual void OnFrameCaptured_RenderingThread(const FCaptureBaseData& InBaseData, TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData, void* InBuffer, int32 Width, int32 Height, int32 BytesPerRow) override;
	virtual bool PostInitializeCaptureViewport(TSharedPtr<FSceneViewport>& InSceneViewport) override;
	virtual bool PostInitializeCaptureRenderTarget(UTextureRenderTarget2D* InRenderTarget) override;
	

	void StopCaptureImpl(bool bAllowPendingFrameToBeProcess) override;
private:
	class fcIMP4Context* RecorderContext;
	
	uint32 RecordedFrames;
	uint32 TargetFrameRate;

	bool bEnableSampling;
	FIntPoint SampledSize;
	TSharedPtr<class CVImage> SourceImage;

	FTimecode BaseTimecode;
	FFrameRate BaseTimecodeFramerate;
	double BaseTimestamp;

	class FVideoEncoderThread* EncoderThread;

	TFunction<void()> OnRecordingDurationExceededWrapper;
 };
