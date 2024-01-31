// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneManagerCFX.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "InstancedFoliageActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "CivilFXCore/CommonCore/PhaseManager.h"
#include "CivilFXCore/CommonCore/PhaseElement.h"

#define MSAA_CMD TEXT("r.MSAACount")

PRAGMA_DISABLE_OPTIMIZATION
void USceneManagerCFX::Initialize(FSubsystemCollectionBase& Collection)
{
	bLabelsEnabled = true;
	bExistingFoliageEnabled = true;
	bProposedFoliageEnabled = true;
	bPedEnabled = false;
	bRTEnabled = true;

#if WITH_EDITOR
	if (!IsTemplate())
	{
		CachedMSAACount = UKismetSystemLibrary::GetConsoleVariableIntValue(MSAA_CMD);
	}
#endif
}

void USceneManagerCFX::Deinitialize()
{
#if WITH_EDITOR
	if (!IsTemplate())
	{
		if (UWorld* World = GetWorld())
		{
			FString Command = FString::Printf(TEXT("%s %d"), MSAA_CMD, CachedMSAACount);
			GEngine->Exec(GEngine->GetWorld(), *Command);
		}
	}
#endif
}

void USceneManagerCFX::SetSceneFOV(float InFoV)
{
	FoV = InFoV;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetFirstPlayerController()->PlayerCameraManager->SetFOV(FoV);
	}
}

float USceneManagerCFX::GetSceneFOV() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		return 	World->GetFirstPlayerController()->PlayerCameraManager->GetFOVAngle();
	}
	return FoV;
}

void USceneManagerCFX::SetExistingFoliageEnabled(bool bInEnabled)
{
	UPhaseManager* PhaseManager = GetWorld()->GetGameInstance()->GetSubsystem<UPhaseManager>();

	if (bInEnabled != bExistingFoliageEnabled)
	{
		bExistingFoliageEnabled = bInEnabled;

		//if (CachedFoliages.Num() == 0)
		//{
			//https://forums.unrealengine.com/t/solved-get-all-actors-from-sublevels/120606/3
		const TArray<ULevelStreaming*>& streamedLevels = GetWorld()->GetStreamingLevels();
		for (ULevelStreaming* streamingLevel : streamedLevels)
		{
			FString levelName = streamingLevel->GetWorldAssetPackageName();

			TArray<AActor*> ExistingFoliages;
			UGameplayStatics::GetAllActorsOfClass(streamingLevel, AInstancedFoliageActor::StaticClass(), ExistingFoliages);
			for (AActor* Foliage : Foliages)
			{
				UPhaseElement* PhaseElement = (UPhaseElement*)Foliage->GetComponentByClass(UPhaseElement::StaticClass());

				if (PhaseElement)
				{
					for (EPhaseType Phase : PhaseElement->PhaseTypes)
					{
						if (Phase == EPhaseType::Existing)
							Foliage->SetActorHiddenInGame(!bExistingFoliageEnabled);
					}
				}

				//else
				//	CachedFoliages.AddUnique(Foliage);
			}
		}
		//~
	}

		//for (AActor* Foliage : CachedFoliages)
		//{
		//	Foliage->SetActorHiddenInGame(!bFoliageEnabled);
		//}
	//}
}

void USceneManagerCFX::SetProposedFoliageEnabled(bool bInEnabled)
{
	UPhaseManager* PhaseManager = GetWorld()->GetGameInstance()->GetSubsystem<UPhaseManager>();

	if (bInEnabled != bProposedFoliageEnabled)
	{
		bProposedFoliageEnabled = bInEnabled;

		//if (CachedFoliages.Num() == 0)
		//{
			//https://forums.unrealengine.com/t/solved-get-all-actors-from-sublevels/120606/3
		const TArray<ULevelStreaming*>& streamedLevels = GetWorld()->GetStreamingLevels();
		for (ULevelStreaming* streamingLevel : streamedLevels)
		{
			FString levelName = streamingLevel->GetWorldAssetPackageName();

			TArray<AActor*> ProposedFoliages;
			UGameplayStatics::GetAllActorsOfClass(streamingLevel, AInstancedFoliageActor::StaticClass(), ProposedFoliages);
			for (AActor* Foliage : Foliages)
			{
				UPhaseElement* PhaseElement = (UPhaseElement*)Foliage->GetComponentByClass(UPhaseElement::StaticClass());

				if (PhaseElement)
				{
					for (EPhaseType Phase : PhaseElement->PhaseTypes)
					{
						if (Phase == EPhaseType::Proposed)
							Foliage->SetActorHiddenInGame(!bProposedFoliageEnabled);
					}
				}

				//else
				//	CachedFoliages.AddUnique(Foliage);
			}

			TArray<AActor*> ProposedGrates;
			UGameplayStatics::GetAllActorsOfClassWithTag(streamingLevel, AInstancedFoliageActor::StaticClass(), ProposedGrates);

		}
		//~
	}

	//for (AActor* Foliage : CachedFoliages)
	//{
	//	Foliage->SetActorHiddenInGame(!bFoliageEnabled);
	//}
//}
}


bool USceneManagerCFX::GetExistingFoliageEnabled() const
{
	return bExistingFoliageEnabled;
}

bool USceneManagerCFX::GetProposedFoliageEnabled() const
{
	return bProposedFoliageEnabled;
}

void USceneManagerCFX::SetLabelsEnabled(bool bInEnabled)
{
	UPhaseManager* PhaseManager = GetWorld()->GetGameInstance()->GetSubsystem<UPhaseManager>();

	if (bInEnabled != bLabelsEnabled)
	{
		bLabelsEnabled = bInEnabled;

		if (CachedLabels.Num() == 0)
		{
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), TEXT("Label"), CachedLabels);
		}
		for (AActor* Label : CachedLabels)
		{
			UPhaseElement* PhaseElement = (UPhaseElement*)Label->GetComponentByClass(UPhaseElement::StaticClass());

			if (PhaseElement)
			{
				for (EPhaseType Phase : PhaseElement->PhaseTypes)
				{
					if (Phase == PhaseManager->GetCurrentPhase())
						Label->SetActorHiddenInGame(!bLabelsEnabled);
				}
			}

			else
				Label->SetActorHiddenInGame(!bLabelsEnabled);
		}
	}
}


bool USceneManagerCFX::GetLabelsEnabled() const
{
	return bLabelsEnabled;
}


void USceneManagerCFX::SetRayTracingEnabled(bool bInEnabled)
{
	FString RTCommandString;

	bRTEnabled = bInEnabled;

	if (bRTEnabled)
		RTCommandString = "r.raytracing.ForceAllRayTracingEffects -1";

	else
		RTCommandString = "r.raytracing.ForceAllRayTracingEffects 0";

	const TCHAR* RTCommand = *RTCommandString;

	GetWorld()->Exec(GetWorld(), RTCommand);
}


bool USceneManagerCFX::GetRayTracingEnabled() const
{
	return bRTEnabled;
}

void USceneManagerCFX::SetTrafficEnabled(bool bInEnabled)
{
	bTrafficEnabled = bInEnabled;
}


bool USceneManagerCFX::GetTrafficEnabled() const
{
	return bTrafficEnabled;
}


//
//void USceneManagerCFX::SetAAOption(int32 OptionIndex)
//{
//	int32 MSAACount = 4;
//	switch (OptionIndex)
//	{
//		case 0:
//			MSAACount = 4;
//			break;
//		case 1:
//			MSAACount = 8;
//			break;
//		case 2:
//			MSAACount = 0; //TAA
//			break;
//	}
//	FString Command = FString::Printf(TEXT("%s %d"), MSAA_CMD, MSAACount);
//	GetWorld()->GetFirstPlayerController()->ConsoleCommand(Command);
//}
PRAGMA_ENABLE_OPTIMIZATION