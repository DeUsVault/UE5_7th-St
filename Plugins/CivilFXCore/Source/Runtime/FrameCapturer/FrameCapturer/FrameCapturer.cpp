// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrameCapturer.h"

#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FFrameCapturer"

void FFrameCapturer::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

//#if WITH_EDITOR
	FcCoreDLLHandle =LoadDLL(TEXT("fccore.dll"));
//#endif
}

void FFrameCapturer::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
//#if WITH_EDITOR
	FPlatformProcess::FreeDllHandle(FcCoreDLLHandle);
//#endif
}

void* FFrameCapturer::LoadDLL(const TCHAR* DLLName)
{
	static const FString BaseDir = IPluginManager::Get().FindPlugin(TEXT("CivilFXCore"))->GetBaseDir();
	static const FString FcCoreLibsDir = FPaths::Combine(BaseDir, TEXT("Binaries"), TEXT("ThirdParty"), TEXT("FcCore"), TEXT("Win64"));
	FPlatformProcess::PushDllDirectory(*FcCoreLibsDir);
	void* Handle = FPlatformProcess::GetDllHandle(*FPaths::Combine(FcCoreLibsDir, DLLName));
	FPlatformProcess::PopDllDirectory(*FcCoreLibsDir);

	UE_LOG(LogTemp, Log, TEXT("Loading dll: %s -> %s"), DLLName, (Handle != nullptr ? TEXT("Success") : TEXT("Failed")));

	return Handle;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFrameCapturer, FrameCapturer)