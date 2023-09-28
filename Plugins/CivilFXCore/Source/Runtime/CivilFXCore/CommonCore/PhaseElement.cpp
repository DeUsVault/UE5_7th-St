// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseElement.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

#include "Engine/Engine.h"

#if WITH_EDITOR
#include "ScopedTransaction.h"
#endif

// Sets default values for this component's properties
UPhaseElement::UPhaseElement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPhaseElement::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UPhaseElement::DoPhase(EPhaseType InPhaseType, EPhaseMode InPhaseMode)
{
	if (InPhaseMode == EPhaseMode::Single)
	{
		SetOwnerActive(bReverse);
	}

	for (EPhaseType PhaseType : PhaseTypes)
	{
		if (PhaseType == InPhaseType)
		{
			SetOwnerActive(!bReverse);
			break;
		}
	}
}

void UPhaseElement::SetOwnerActive(bool bActive)
{
#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "PhaseElement"
	const FScopedTransaction Transaction(LOCTEXT("PhaseElement", "Phasing Element(s)"));
#endif

	AActor* Owner = GetOwner();

	TInlineComponentArray<USceneComponent*> ComponentArray;
	Owner->GetComponents(ComponentArray);
	for (USceneComponent* Component : ComponentArray)
	{
		Component->Modify();
		Component->SetVisibility(bActive);
		/*
		//Delay of BeginPlay() and Tick() should be implemented in the component
		Component->SetActive(bActive);
		Component->SetHiddenInGame(!bActive);
		Component->SetComponentTickEnabled(bActive);
		*/

	}
	Owner->Modify();
	Owner->SetActorHiddenInGame(!bActive);
	Owner->SetActorEnableCollision(bActive);
	Owner->SetActorTickEnabled(bActive);

#if WITH_EDITOR
#undef LOCTEXT_NAMESPACE
#endif
}

/*
// Called every frame
void UPhaseElement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
*/
