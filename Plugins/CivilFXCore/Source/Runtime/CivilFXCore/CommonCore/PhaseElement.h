// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhaseManager.h"
#include "PhaseElement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CIVILFXCORE_API UPhaseElement : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhaseElement();

	UPROPERTY(EditAnywhere)
	TArray<EPhaseType> PhaseTypes;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bReverse;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void DoPhase(EPhaseType InPhaseType, EPhaseMode InPhaseMode);
private:
	void SetOwnerActive(bool bActive);
};
