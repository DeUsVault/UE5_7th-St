// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CivilFXCore/Subsystems/SceneManagerCFX.h"
#include "PhaseManager.generated.h"


/**
 * IMPORTANT: Do not alter display name,
 * i.e., with UMETA(DisplayName = "...")
 * as the name of enum won't work correctly at runtime
 */
UENUM()
enum class EPhaseType : uint8
{
	Proposed = 0,
	None
};

UENUM()
enum class EPhaseMode : uint8
{
	Single = 0,
	Additive
};

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UPhaseManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	DECLARE_EVENT_TwoParams(UPhaseManager, FOnPhaseSwitched, EPhaseType, EPhaseMode)
	FOnPhaseSwitched& OnSwitchPhase() { return PhaseSwitchedEvent; }

	/**
		Call to switch to a new phase.
		Does not have effect if the new phase is already an active phase
		@params:
			EPhaseType: New Type
			EPhaseMode: New Mode
	*/
	void SwitchPhase(EPhaseType InPhaseType, EPhaseMode InPhaseMode = EPhaseMode::Single);
	EPhaseType GetCurrentPhase() const;

protected:

	/*subsystem interfaces*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	FOnPhaseSwitched PhaseSwitchedEvent;
	TArray<EPhaseType> CurrentPhases;
	EPhaseType CurrentPhase;
};
