// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "VideoEncoderThreadManager.h"

void FVideoEncoderThreadManager::CreateInternal(class fcIMP4Context* InRecorderContext, int32 InThreadCount)
{
	ThreadCount = InThreadCount;

	for (int32 i = 0; i < ThreadCount; ++i)
	{
		FVideoEncoderThread* NewThread = new FVideoEncoderThread;
		FString ThreadName = FString::Printf(TEXT("VideoEncoderThread %d"), i);
		NewThread->Create(ThreadName, InRecorderContext, TPri_Highest);

		EncoderThreads.Add(NewThread);
	}
}

void FVideoEncoderThreadManager::CleanupInternal()
{
	for (FVideoEncoderThread* EncoderThr : EncoderThreads)
	{
		EncoderThr->KillThread();
		delete EncoderThr;
	}
}

FVideoEncoderThreadManager* FVideoEncoderThreadManager::InternalThreadManager = nullptr;

FVideoEncoderThreadManager* FVideoEncoderThreadManager::Create(class fcIMP4Context* InRecorderContext, int32 ThreadCount)
{
	check(InternalThreadManager != NULL && "FVideoEncoderThreadManager only supports 1 instance. Did you forget to call ::CleanupAndDestroy()?");

	FVideoEncoderThreadManager* NewManager = new FVideoEncoderThreadManager;

	NewManager->CreateInternal(InRecorderContext, ThreadCount);

	InternalThreadManager = NewManager;
	return InternalThreadManager;
}

void FVideoEncoderThreadManager::CleanupAndDestroy()
{
	InternalThreadManager->CleanupInternal();
	delete InternalThreadManager;
	InternalThreadManager = nullptr;
}

void FVideoEncoderThreadManager::InsertVideoFrameTask(TUniquePtr<FVideoFrameTask>&& InTask)
{
	static int32 LastInsertedTaskThread = 0;



	++LastInsertedTaskThread;
	LastInsertedTaskThread %= ThreadCount;
}
