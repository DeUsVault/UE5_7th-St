// Fill out your copyright notice in the Description page of Project Settings.


#include "PedestrianComponent.h"
#include "Components/SplineComponent.h"
#include "TimerManager.h"
#include "PathSpline.h"
#include "DrawDebugHelpers.h"
#include "Human.h"

// Sets default values for this component's properties
UPedestrianComponent::UPedestrianComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPedestrianComponent::BeginPlay()
{
	Super::BeginPlay();
	
	spawnTimerChangeDelegate.BindUFunction(this, FName("OnSpawnTimerChange"));

	SplineFromParent = Cast<APathSpline>(GetOwner())->SplineComponent;
	CurrentSpawnTime = SpawnTimerMin;
	this->GetWorld()->GetTimerManager().SetTimer(ShouldSpawnMoverHandler, this, &UPedestrianComponent::ShouldSpawnAMover, FMath::RandRange(SpawnTimerMin, SpawnTimerMax), true, -1.0f);
	this->GetWorld()->GetTimerManager().PauseTimer(ShouldSpawnMoverHandler);
	this->GetWorld()->GetTimerManager().SetTimer(UpdatePositionHandler, this, &UPedestrianComponent::UpdatePosition, 0.03f, true, -1.0f);
	this->GetWorld()->GetTimerManager().PauseTimer(UpdatePositionHandler);
	this->GetWorld()->GetTimerManager().SetTimer(MovingObjectRespawnHandler, this, &UPedestrianComponent::MovingObjectRespawn, FMath::RandRange(SpawnTimerMin, SpawnTimerMax), true, -1.0f);
	this->GetWorld()->GetTimerManager().PauseTimer(MovingObjectRespawnHandler);

	// ...

	UPhaseManager* PhaseManager = GetOwner()->GetGameInstance()->GetSubsystem<UPhaseManager>();
	PhaseManager->OnSwitchPhase().AddUObject(this, &UPedestrianComponent::OnNewPhaseSwitch);
	
}

void UPedestrianComponent::ChangeSpawnTime()
{

	this->GetWorld()->GetTimerManager().SetTimer(ShouldSpawnMoverHandler, this, &UPedestrianComponent::ShouldSpawnAMover, SpawnTimerMin, true, -1.0f);
	CurrentSpawnTime = SpawnTimerMin;
}


// Called every frame
void UPedestrianComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPedestrianComponent::UpdatePosition()
{
	int counter = 0;

	if (GetWorld()->GetTimerManager().IsTimerPaused(ShouldSpawnMoverHandler) &&
		MovingObject.Num() < MaxNumOfPeople)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Unpausing Timer"));
		GetWorld()->GetTimerManager().UnPauseTimer(ShouldSpawnMoverHandler);
	} 

	for (AHuman* mo : MovingObject)
	{
		if (mo != nullptr && mo->bCanMoveObject)
		{
			if (mo->IsHidden())
			{
				mo->SetActorHiddenInGame(false);
			}
			counter++;
			//mo->SetCurrentSplineTime((GetWorld()->GetTimeSeconds() - mo->StartTime) / SecondsToCompletePath);
			float distance = 0.0f;

			if(mo->MovingFromSplineStart)
			{
				distance = mo->CalculateDistance(GetWorld()->GetTimeSeconds());
				//UE_LOG(LogTemp, Warning, TEXT("%s distance %f"), *GetOwner()->GetName(), distance);
			}
			else
			{
				distance = SplineFromParent->GetSplineLength() - mo->CalculateDistance(GetWorld()->GetTimeSeconds());
				//UE_LOG(LogTemp, Warning, TEXT("%s distance %f"), *GetOwner()->GetName(), distance, distance);
			}

			FVector position = SplineFromParent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
			FVector direction = SplineFromParent->GetDirectionAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
			/*************************************************/
			/* Adjusting for offset */
			FVector leftDir = FVector::CrossProduct(direction, FVector::UpVector);
			FVector leftWidth = leftDir * (mo->offsetFromSpline);
			float seg = 0.9f;
			FVector adjustedPos = position;
			/*************************************************/
			FRotator rotator = UKismetMathLibrary::MakeRotationFromAxes(direction, -leftDir, FVector::UpVector);
			if (!mo->MovingFromSplineStart)
			{
				rotator.Yaw += 180.0f;
				rotator.Pitch *= -1;
				//position -= FVector(mo->offsetFromSpline, mo->offsetFromSpline, 0);
				adjustedPos = FMath::Lerp(position - leftWidth, position + leftWidth, seg);
				//UE_LOG(LogTemp, Warning, TEXT("%s Starting from End"), *GetOwner()->GetName());
			}
			else
			{
				//position += FVector(mo->offsetFromSpline, mo->offsetFromSpline, 0);
				adjustedPos = FMath::Lerp(position + leftWidth, position - leftWidth, seg);
				//UE_LOG(LogTemp, Warning, TEXT("%s Starting from spline start"), *GetOwner()->GetName());
			}

			if(bTrackFloor)
			{
				adjustedPos = AdjustLocationToFloor(adjustedPos);
			}
			//DrawDebugSphere(GetWorld(), position + newLoc + FVector(100.0f, 0.0f, 0.0f), 50, 50, FColor::Blue);
			mo->SetActorLocation(adjustedPos);
			//UE_LOG(LogTemp, Warning, TEXT("positionX %f,%f,%f"), position.X,position.Y,position.Z);
			mo->SetActorRotation(rotator);

			if ((distance >= SplineFromParent->GetSplineLength() && mo->MovingFromSplineStart) ||
				(distance <= 0 && !mo->MovingFromSplineStart))
			{
				if (bLoopPedestrians)
				{
					mo->bCanMoveObject = false;	
					float nextRespawnTime = FMath::RandRange(SpawnTimerMin, SpawnTimerMax);
					//UE_LOG(LogTemp, Warning, TEXT("nextRespawnTime %f"), nextRespawnTime);
					this->GetWorld()->GetTimerManager().SetTimer(MovingObjectRespawnHandler, this, &UPedestrianComponent::MovingObjectRespawn, nextRespawnTime, true, -1.0f);

				}
				mo->SetActorHiddenInGame(true);
			}
		}
		else
		{

			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Moving Object is null!!!"));
		}
	}

	//if (SpawnTimerMin != CurrentSpawnTime)
	//{
	//	ChangeSpawnTime();
	//}

}

void UPedestrianComponent::SpawnAMover(TSubclassOf<AActor> moc)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s:: APedestrian::SpawnAMover is called"), *GetOwner()->GetName());
	AHuman* newActor = GetWorld()->SpawnActor<AHuman>(moc, SplineFromParent->GetTransformAtSplinePoint(0, ESplineCoordinateSpace::World));
	newActor->offsetFromSpline = offsetFromSpline;

	if (newActor != nullptr)
	{
		//newActor->SetActorLocation(SplineComponent->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::Local));
		newActor->StartTime = GetWorld()->GetTimeSeconds();
		if(bTwoWayPedestrian)
		{
			newActor->MovingFromSplineStart = FMath::RandRange(0, 1) == 1 ? true : false;
		}
		else // One way walkway so spline always start at spline start
		{
			
			newActor->MovingFromSplineStart = true;
		}
		newActor->bCanMoveObject = true;
		newActor->SetActorHiddenInGame(true);
		MovingObject.Add(newActor);
		//UE_LOG(LogTemp, Warning, TEXT("Added a new Actor. Total number of objects %d"), MovingObject.Num());
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Moving object is null"));
	}
}

void UPedestrianComponent::ShouldSpawnAMover()
{
	//UE_LOG(LogTemp, Warning, TEXT("APedestrian::ShouldSpawnAMover is called"));
	//UE_LOG(LogTemp, Warning, TEXT("APedestrian:: MovingObject.Num() %d MaxNumberOfPedestrian %d"), MovingObject.Num(), MaxNumOfPeople);

	if (MovingObjectClass.Num() == 0)
	{
		return;
	}

	if (MovingObject.Num() < MaxNumOfPeople)
	{
		uint32 randomSapwnMover = FMath::RandRange(0, MovingObjectClass.Num() - 1);
		SpawnAMover(MovingObjectClass[randomSapwnMover]);

		this->GetWorld()->GetTimerManager().SetTimer(ShouldSpawnMoverHandler, this, &UPedestrianComponent::ShouldSpawnAMover, FMath::RandRange(SpawnTimerMin, SpawnTimerMax), true, -1.0f);
	}
	else
	{
		if (ShouldSpawnMoverHandler.IsValid())
		{
			GetWorld()->GetTimerManager().PauseTimer(ShouldSpawnMoverHandler);
			//UE_LOG(LogTemp, Warning, TEXT("ShouldSpawnMoverHandler is paused"));
		}
	}
}

void UPedestrianComponent::MovingObjectRespawn()
{
	if (MovingObject.Num() > 0 &&
		CurrentMoverIndex >= 0 && 
		CurrentMoverIndex < MovingObject.Num() &&
		CurrentMoverIndex < MaxNumOfPeople)
	{
		
		AHuman* moa = MovingObject[CurrentMoverIndex];
		if (!moa->bCanMoveObject)
		{
			moa->StartTime = GetWorld()->GetTimeSeconds();
			moa->SetActorHiddenInGame(true);
			moa->bCanMoveObject = true;
			//UE_LOG(LogTemp, Warning, TEXT("Respawning CurrentMoverIndex %d"), CurrentMoverIndex);
			CurrentMoverIndex = (CurrentMoverIndex + 1) % MaxNumOfPeople;
			//UE_LOG(LogTemp, Warning, TEXT("Respawning :: Increasing CurrentMoverIndex %d"), CurrentMoverIndex);
		}

	}
}

void UPedestrianComponent::TestFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling TestFunction"));
}

void UPedestrianComponent::OnSpawnTimerChange()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling OnSpawnTimerChange"));
}


void UPedestrianComponent::OnNewPhaseSwitch(EPhaseType NewPhaseType, EPhaseMode NewPhaseMode)
{
	//GLog->Log(FString::Printf(TEXT("NewPhase for Actor: %s, ThisPhaseType: %s, NewPhaseType: %s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(PhaseType), *UEnum::GetValueAsString(NewPhaseType)));

	//UE_LOG(LogTemp, Warning, TEXT("NewPhase for Actor:% s, ThisPhaseType : % s, NewPhaseType : % s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(PhaseType), *UEnum::GetValueAsString(NewPhaseType));
	if (NewPhaseType == PhaseType)
	{	
		//UE_LOG(LogTemp, Warning, TEXT("Pausing Timers"));

		GetWorld()->GetTimerManager().UnPauseTimer(UpdatePositionHandler);
		GetWorld()->GetTimerManager().UnPauseTimer(ShouldSpawnMoverHandler);
		GetWorld()->GetTimerManager().UnPauseTimer(MovingObjectRespawnHandler);
		ResetLocation();
	}
	else
	{
		HideAllPedestrians();
		GetWorld()->GetTimerManager().PauseTimer(UpdatePositionHandler);
		GetWorld()->GetTimerManager().PauseTimer(ShouldSpawnMoverHandler);
		GetWorld()->GetTimerManager().PauseTimer(MovingObjectRespawnHandler);
	}
}

void UPedestrianComponent::ResetLocation()
{
	for(AHuman* moa : MovingObject)
	{
		//moa->StartTime = GetWorld()->GetTimeSeconds();
		moa->SetActorHiddenInGame(false);
		moa->bCanMoveObject = true;
		//CurrentMoverIndex = 0;
	}
}

void UPedestrianComponent::HideAllPedestrians()
{
	for (AHuman* moa : MovingObject)
	{
		moa->SetActorHiddenInGame(true);
		moa->bCanMoveObject = false;
	}
}

FVector UPedestrianComponent::AdjustLocationToFloor(FVector location)
{
	FHitResult outHitResult;
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(PedestrianActor), true);

	//UE_LOG(LogTemp, Warning, TEXT("AdjustLocationToFloor"));

	if (GetWorld()->LineTraceSingleByChannel(outHitResult, location + FVector::UpVector * 100, location + FVector::DownVector * 1000.0f, ECC_WorldStatic, CollisionParams))
	{

		//DrawDebugLine(GetWorld(), location, location + FVector::DownVector * 1000.0f, FColor::Blue, true, 1.0f);
		//DrawDebugSphere(GetWorld(), location + FVector::UpVector * 100, 5.0f, 1, FColor::Red, false, 1.0f);
		//UE_LOG(LogTemp, Warning, TEXT("Hit down vector %f %f"), location.Z, outHitResult.ImpactPoint.Z);
		return outHitResult.ImpactPoint;
	}
	else if (GetWorld()->LineTraceSingleByChannel(outHitResult, location, location + FVector::UpVector * 1000.0f, ECC_WorldStatic, CollisionParams))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hit Up vector %f"), outHitResult.Location.Z);
		return outHitResult.ImpactPoint;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Hit"));
		return location;
	}

}

