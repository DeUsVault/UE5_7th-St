// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainControllerComponent.h"
#include "Components/SplineComponent.h"
#include "MovingObjectActor.h"
#include "PathSpline.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CivilFXCore/CommonCore/PhaseElement.h"


// Sets default values for this component's properties
UTrainControllerComponent::UTrainControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTrainControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(UpdatePositionHandler, this, &UTrainControllerComponent::UpdatePosition, 0.02f, true, -1.0f);
	GetWorld()->GetTimerManager().PauseTimer(UpdatePositionHandler);
	SplineFromParent = Cast<APathSpline>(GetOwner())->SplineComponent;
	bParentSplineIsLooping = Cast<APathSpline>(GetOwner())->bPathIsLooping;


	// Make sure that the object that will be moving is not null, initialize the object that will be moving and initialize the
	// time the application started. Only then the object that will be moving can move.
	for (TSubclassOf<AActor> moc : MovingObjectClass)
	{
		if (moc != nullptr)
		{
			AMovingObjectActor* newActor = GetWorld()->SpawnActor<AMovingObjectActor>(moc, SplineFromParent->GetComponentTransform());

			if (newActor != nullptr)
			{
				newActor->StartTime = GetWorld()->GetTimeSeconds();
				newActor->bCanMoveObject = true;
				MovingObject.Add(newActor);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Moving object is null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Moving object class is null"));
		}
	}

	PhaseManager = GetOwner()->GetGameInstance()->GetSubsystem<UPhaseManager>();
	PhaseManager->OnSwitchPhase().AddUObject(this, &UTrainControllerComponent::OnNewPhaseSwitch);

	HideAllTrainCars();
	GetWorld()->GetTimerManager().SetTimer(DelayHandler, this, &UTrainControllerComponent::DelayTrain, DelayTime, false, -1.0f);
}

void UTrainControllerComponent::MovingObjectRespawn()
{
	for (AMovingObjectActor* moa : MovingObject)
	{
		moa->StartTime = GetWorld()->GetTimeSeconds();
		moa->bCanMoveObject = true;
		moa->SetActorHiddenInGame(false);
	}
}

void UTrainControllerComponent::UpdatePosition()
{

	int counter = 0;
	for (AMovingObjectActor* mo : MovingObject)
	{
		if (mo != nullptr && mo->bCanMoveObject)
		{
			counter++;
			CurrentSplineTime = (GetWorld()->GetTimeSeconds() - mo->StartTime) / SecondsToCompletePath;
			float distance = (SplineFromParent->GetSplineLength() * CurrentSplineTime) - mo->offset;

			FVector position = SplineFromParent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
			mo->SetActorLocation(position);

			FVector direction = SplineFromParent->GetDirectionAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
			FRotator rotator = FRotationMatrix::MakeFromX(direction).Rotator();
			mo->SetActorRotation(rotator);

			if (CurrentSplineTime >= 1.0f)
			{
				if (bParentSplineIsLooping)
				{
					if (bPauseInBetweenLoops)
					{
						this->GetWorld()->GetTimerManager().SetTimer(MovingObjectRespawnHandler, this, &UTrainControllerComponent::MovingObjectRespawn, PauseTimeInSeconds, false, -1.0f);
						mo->bCanMoveObject = false;
					}
					else
					{
						MovingObjectRespawn();
						mo->bCanMoveObject = true;
					}
				}
			}
		}
		else
		{

			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Moving Object is null!!!"));
		}
	}


}


// Called every frame
void UTrainControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UTrainControllerComponent::OnNewPhaseSwitch(EPhaseType NewPhaseType, EPhaseMode NewPhaseMode)
{
	//GLog->Log(FString::Printf(TEXT("NewPhase for Actor: %s, ThisPhaseType: %s, NewPhaseType: %s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(PhaseType), *UEnum::GetValueAsString(NewPhaseType)));

	//UE_LOG(LogTemp, Warning, TEXT("NewPhase for Actor:% s, ThisPhaseType : % s, NewPhaseType : % s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(PhaseType), *UEnum::GetValueAsString(NewPhaseType));
	if (NewPhaseType == PhaseType)
	{
		GetWorld()->GetTimerManager().SetTimer(DelayHandler, this, &UTrainControllerComponent::DelayTrain, DelayTime, false, -1.0f);

		//UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), TEXT("Train"), CachedTrains);

		//for (AActor* Train : CachedTrains)
		//{
		//	UPhaseElement* PhaseElement = (UPhaseElement*)Train->GetComponentByClass(UPhaseElement::StaticClass());

		//	for (EPhaseType Phase : PhaseElement->PhaseTypes)
		//	{
		//		if (Phase == NewPhaseType)
		//		{
		//			// do nothing
		//			return;
		//		}
		//	}	

		//	UTrainControllerComponent* TrainController = (UTrainControllerComponent*)Train->GetComponentByClass(UTrainControllerComponent::StaticClass());
		//	TrainController->HideAllTrainCars();
		//}
	}

	else
	{
		HideAllTrainCars();
	}
}

void UTrainControllerComponent::HideAllTrainCars()
{
	GetWorld()->GetTimerManager().PauseTimer(UpdatePositionHandler);

	for (AMovingObjectActor* moa : MovingObject)
	{
		moa->SetActorHiddenInGame(true);
	}
}

void UTrainControllerComponent::DelayTrain()
{
	// Check to make sure still on correct phase when this activates
	if (PhaseManager->GetCurrentPhase() == PhaseType)
	{
		MovingObjectRespawn();
		GetWorld()->GetTimerManager().UnPauseTimer(UpdatePositionHandler);
	}
}

