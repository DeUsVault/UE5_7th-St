// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "VideoEncoderThread.h"
#include "HAL/Event.h"
#include "HAL/PlatformProcess.h"


/**/


FVideoEncoderThread::FVideoEncoderThread() :
	FramesCounter(0),
	DoWorkEvent(nullptr),
	TimeToDie(false),
	Thread(nullptr)
{
}

uint32 FVideoEncoderThread::Run()
{
	while (!TimeToDie.Load(EMemoryOrder::Relaxed))
	{
		bool bContinueWaiting = true;

		if (bContinueWaiting)
		{
			GLog->Log("Sleeping");
			DoWorkEvent->Wait();
		}

		FPlatformMisc::MemoryBarrier();
		while (!VideoFrameTaskQueue.IsEmpty())
		{
			--FramesCounter;

			TUniquePtr<FVideoFrameTask> Task;
			VideoFrameTaskQueue.Dequeue(Task);

			const void* PixelData = nullptr;
			int64 DataSize = 0;

			Task->PixelData->RetrieveData(PixelData, DataSize);
			GLog->Log(FString::Printf(TEXT("[%s] Timestamp: %f"),*ThreadName, Task->Timestamp));
			fcMP4AddVideoFramePixels(RecorderContext, PixelData, fcPixelFormat::fcPixelFormat_RGBAf16, Task->Timestamp);
		}

	}
	return 0;
}


bool FVideoEncoderThread::Create(FString InThreadName, fcIMP4Context* InRecorderContext, EThreadPriority ThreadPriority/*=TPri_Normal*/)
{
	ThreadName = InThreadName;

	RecorderContext = InRecorderContext;

	DoWorkEvent = FPlatformProcess::GetSynchEventFromPool();
	Thread = FRunnableThread::Create(this, *ThreadName, 0, ThreadPriority, FPlatformAffinity::GetPoolThreadMask());
	check(Thread);
	return true;
}

bool FVideoEncoderThread::KillThread()
{
	bool bDidExitOK = true;

	TimeToDie = true;

	DoWorkEvent->Trigger();

	Thread->WaitForCompletion();

	FPlatformProcess::ReturnSynchEventToPool(DoWorkEvent);
	DoWorkEvent = nullptr;

	delete Thread;
	return bDidExitOK;
}

void FVideoEncoderThread::EnqueueWork(TUniquePtr<FVideoFrameTask>&& InTask)
{
	VideoFrameTaskQueue.Enqueue(MoveTemp(InTask));
	++FramesCounter;
	FPlatformMisc::MemoryBarrier();
	DoWorkEvent->Trigger();
}

void FVideoEncoderThread::SyncWithRenderingThread()
{
	Thread->WaitForCompletion();
}

