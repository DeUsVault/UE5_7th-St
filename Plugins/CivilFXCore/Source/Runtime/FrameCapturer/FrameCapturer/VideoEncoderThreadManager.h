// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "VideoEncoderThread.h"
/**
 * 
 */
class FRAMECAPTURER_API FVideoEncoderThreadManager
{
public:
	
	static FVideoEncoderThreadManager* Create(class fcIMP4Context* InRecorderContext, int32 ThreadCount);
	static void CleanupAndDestroy();

	void InsertVideoFrameTask(TUniquePtr<FVideoFrameTask>&& InTask);

private:
	void CreateInternal(class fcIMP4Context* InRecorderContext, int32 ThreadCount);
	void CleanupInternal();
	TArray<FVideoEncoderThread*> EncoderThreads;
	int32 ThreadCount;


	static FVideoEncoderThreadManager* InternalThreadManager;
};
