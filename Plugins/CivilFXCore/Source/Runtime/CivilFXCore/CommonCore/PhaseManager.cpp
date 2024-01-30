// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseManager.h"
#include "GameFramework/Actor.h"
#include "PhaseElement.h"
#include "UObject/UObjectIterator.h"


void UPhaseManager::Initialize(FSubsystemCollectionBase& Collection)
{
	
}

void UPhaseManager::Deinitialize()
{
}

void UPhaseManager::SwitchPhase(EPhaseType InPhaseType, EPhaseMode InPhaseMode)
{
	USceneManagerCFX* SceneManager = GetWorld()->GetSubsystem<USceneManagerCFX>();

	CurrentPhase = InPhaseType;
	//GLog->Log(FString::Printf(TEXT("Switching phase: %s"), *UEnum::GetValueAsString(InPhaseType)));

	if (!CurrentPhases.Contains(InPhaseType))
	{
		
		for (TObjectIterator<AActor> ActorIt; ActorIt; ++ActorIt)
		{
			AActor* CurrentActor = *ActorIt;
			UPhaseElement* PhaseElement = (UPhaseElement*)CurrentActor->GetComponentByClass(UPhaseElement::StaticClass());
			if (PhaseElement)
			{
				if (CurrentActor->ActorHasTag("Label") && !SceneManager->GetLabelsEnabled())
				{
					break;
				}

				else if (CurrentActor->ActorHasTag("Existing Foliage") && !SceneManager->GetExistingFoliageEnabled())
				{
					break;
				}

				else if (CurrentActor->ActorHasTag("Proposed Foliage") && !SceneManager->GetProposedFoliageEnabled())
				{
					break;
				}

				else
					PhaseElement->DoPhase(InPhaseType, InPhaseMode);
			}
		}
		PhaseSwitchedEvent.Broadcast(InPhaseType, InPhaseMode);
	}

	if (InPhaseMode == EPhaseMode::Single)
	{
		CurrentPhases.Empty();
	}
	CurrentPhases.Add(InPhaseType);
}

EPhaseType UPhaseManager::GetCurrentPhase() const
{
	return CurrentPhase;
}
