// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "MediaOutput.h"

#include "Engine/RendererSettings.h"
#include "Misc/FrameRate.h"

#include "CFXMediaOutput.generated.h"

/** Texture format supported */
UENUM()
enum class ECFXMediaOutputPixelFormat
{
	B8G8R8A8					UMETA(DisplayName = "8bit RGBA"),
	FloatRGBA					UMETA(DisplayName = "Float RGBA"),
};

USTRUCT()
struct FImageSamplingOptions
{
	GENERATED_BODY()

	bool bUseSampling;
	FIntPoint SourceSize;
	FIntPoint SampledSize;
};



UCLASS()
class FRAMECAPTURER_API UCFXMediaOutput : public UMediaOutput
{
	GENERATED_BODY()

public:

	FImageSamplingOptions SamplingOptions;

	FIntPoint DesiredSize;
	FFrameRate DesiredFrameRate;

	FFrameRate GetRequestedFrameRate() const;

	//~ UMediaOutput interface
	virtual bool Validate(FString& FailureReason) const override;
	virtual FIntPoint GetRequestedSize() const override;
	virtual EPixelFormat GetRequestedPixelFormat() const override;
	virtual EMediaCaptureConversionOperation GetConversionOperation(EMediaCaptureSourceType InSourceType) const override;

protected:
	virtual UMediaCapture* CreateMediaCaptureImpl() override;

};
