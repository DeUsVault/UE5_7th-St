// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "HAL/Runnable.h"
#include "HAL/PlatformTLS.h"
#include "HAL/PlatformAffinity.h"
#include "HAL/RunnableThread.h"
#include "Misc/IQueuedWork.h"

#include "ImagePixelData.h"

#include "Containers/Queue.h"

#include "fccore.h"


class FVideoFrameTask
{
public:
	TUniquePtr<TImagePixelData<FFloat16Color>> PixelData;
	double Timestamp;
};

/**
 * 
 */
class FRAMECAPTURER_API FVideoEncoderThread : public FRunnable
{
public:
	FVideoEncoderThread();
	~FVideoEncoderThread() {}
	uint32 Run() override;

	virtual bool Create(FString InThreadName, class fcIMP4Context* InRecorderContext, EThreadPriority ThreadPriority=TPri_Normal);
	virtual bool KillThread();
	virtual void EnqueueWork(TUniquePtr<FVideoFrameTask>&& InTask);
	virtual void SyncWithRenderingThread();

	TAtomic<int32> FramesCounter;

protected:
	FString ThreadName;

	/** The event that tells the thread there is work to do. */
	FEvent* DoWorkEvent;

	/** If true, the thread should exit. */
	TAtomic<bool> TimeToDie;

	FRunnableThread* Thread;

	class fcIMP4Context* RecorderContext;

	TQueue<TUniquePtr<FVideoFrameTask>, EQueueMode::Spsc> VideoFrameTaskQueue;
};
