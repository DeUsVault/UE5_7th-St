// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "CFXMediaOutput.h"
#include "CFXMediaCapture.h"


FFrameRate UCFXMediaOutput::GetRequestedFrameRate() const
{
	return DesiredFrameRate;
}

bool UCFXMediaOutput::Validate(FString& OutFailureReason) const
{
	if (!Super::Validate(OutFailureReason))
	{
		return false;
	}

	return true;
}

FIntPoint UCFXMediaOutput::GetRequestedSize() const
{
	if (SamplingOptions.bUseSampling)
	{
		return SamplingOptions.SourceSize;
	}
	return DesiredSize;
}

EPixelFormat UCFXMediaOutput::GetRequestedPixelFormat() const
{
	return PF_FloatRGBA;
	//return PF_A2B10G10R10;
}

EMediaCaptureConversionOperation UCFXMediaOutput::GetConversionOperation(EMediaCaptureSourceType InSourceType) const
{
	return EMediaCaptureConversionOperation::NONE;
	//return EMediaCaptureConversionOperation::INVERT_ALPHA;
	//return EMediaCaptureConversionOperation::SET_ALPHA_ONE;
	//return EMediaCaptureConversionOperation::RGBA8_TO_YUV_8BIT;
	//return EMediaCaptureConversionOperation::RGB10_TO_YUVv210_10BIT;
}

UMediaCapture* UCFXMediaOutput::CreateMediaCaptureImpl()
{
	UMediaCapture* Result = NewObject<UCFXMediaCapture>();
	if (Result)
	{
		Result->SetMediaOutput(this);
	}
	return Result;
}
