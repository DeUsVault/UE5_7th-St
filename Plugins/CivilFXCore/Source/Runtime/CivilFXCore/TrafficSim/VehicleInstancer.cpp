// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleInstancer.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "VehicleInstancerManager.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "VehicleInstancerMeasureTool.h"
#include "Windows/WindowsPlatformApplicationMisc.h" //for copy to clipboard
#include "Materials/MaterialInstanceDynamic.h"

/*Wheel Mesh Class*/
int32 FVehicleWheelMesh::CreateInstance(const FTransform& InBodyTransform)
{
	return WheelMesh->AddInstance(AVehicleInstancer::GetWorldTransformFromLocal(InBodyTransform, BaseRotator, OffsetToBody, FVector::OneVector));
}

void FVehicleWheelMesh::UpdateInstanceWorldLocation(int32 Index, const FTransform& InBodyLocation, float InSpeed)
{
	/*
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Updating Vehicle Transform")) + (InBodyLocation.GetLocation().ToString()));
	FVector Location = InBodyLocation.TransformPosition(OffsetToBody);
	//FRotator Rotation = CombineRotators(OffsetAngle, InBodyLocation.GetRotation().Rotator());
	FVector Scale = InBodyLocation.GetScale3D();
	FQuat Rotator = InBodyLocation.TransformRotation(BaseRotator);
	WheelMesh->UpdateInstanceTransform(Index, { Rotator, Location, Scale }, true, true, true);
	*/
	WheelMesh->UpdateInstanceTransform(Index, AVehicleInstancer::GetWorldTransformFromLocal(InBodyLocation, BaseRotator, OffsetToBody, FVector::OneVector));
}

/********************/
/********************/
/*Vehicle Mesh Class*/

// Sets default values
AVehicleInstancer::AVehicleInstancer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BodyMesh = CreateDefaultSubobject<UFoliageInstancedStaticMeshComponent>("Vehicle Body");
	RootComponent = BodyMesh;

	BodyMesh->Modify();
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	BodyMesh->SetGenerateOverlapEvents(false);
	BodyMesh->SetCanEverAffectNavigation(false);
	BodyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	BodyMesh->SetShouldUpdatePhysicsVolume(false);
	BodyMesh->SetSimulatePhysics(false);
	

	DummyMesh = CreateDefaultSubobject<UStaticMeshComponent>("Dummy Mesh");
	DummyMesh->Modify();
	DummyMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	DummyMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

#if WITH_EDITORONLY_DATA
	ToolComponent = CreateDefaultSubobject<UVehicleInstancerMeasureTool>("Measure Tool");
	ToolComponent->Modify();
	ToolComponent->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);
#endif
}


// Called when the game starts or when spawned
void AVehicleInstancer::BeginPlay()
{
	Super::BeginPlay();

	Manager = GetGameInstance()->GetSubsystem<UVehicleInstancerManager>();
	Manager->RegisterInstancedActor(this);

	//init wheels variables too
	for (auto& Wheel : WheelMeshes)
	{
		Wheel.BeginPlay();
	}

	/*
	int32 MatIndex = BodyMesh->GetStaticMesh()->GetMaterialIndexFromImportedMaterialSlotName(TEXT("body"));
	GLog->Log(FString::FromInt(MatIndex));
	UMaterialInterface* MatInterface = BodyMesh->GetStaticMesh()->GetMaterial(MatIndex);
	UMaterialInstanceDynamic* MatDynamic = UMaterialInstanceDynamic::Create(MatInterface, this);

	MatDynamic->SetVectorParameterValue(TEXT("Car Paint Color"), FLinearColor::MakeRandomColor());
	BodyMesh->GetStaticMesh()->SetMaterial(MatIndex, MatDynamic);
	*/
}



int32 AVehicleInstancer::GetOrCreateInstance()
{
	int32 Index;

	//reuse an instance if there is one free
	int32 UnusedInstancesCount = UnusedInstances.Num();
	if (UnusedInstancesCount > 0)
	{
		Index = UnusedInstances[UnusedInstancesCount - 1];
		UnusedInstances.RemoveAt(UnusedInstancesCount - 1);
		return Index;
	}

	/*otherwise add new instance*/
	//create body instanced
	Index = BodyMesh->AddInstance(BaseTransform);
	//create wheels instanced
	for (FVehicleWheelMesh& WheelMesh : WheelMeshes)
	{
		WheelMesh.CreateInstance(BaseTransform);
	}
	return Index;
}

void AVehicleInstancer::ReleaseInstance(int32 Index)
{
	FTransform Transform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::ZeroVector);
	UpdateInstanceWorldLocation(Index, Transform, 0);
	//add instance to free pool
	UnusedInstances.Add(Index);
}

void AVehicleInstancer::UpdateInstanceWorldLocation(int32 Index, const FTransform& InBodyTransform, float InSpeed)
{
	//update body
	BodyMesh->UpdateInstanceTransform(Index, InBodyTransform, true, true, true);

	//update wheels
	for (FVehicleWheelMesh& WheelMesh : WheelMeshes)
	{
		WheelMesh.UpdateInstanceWorldLocation(Index, InBodyTransform, InSpeed);
	}

}

void AVehicleInstancer::SetVisibility(int32 Index, bool bVisible)
{
	FVector Scale = FVector::OneVector;
	if (!bVisible)
	{
		Scale = FVector::ZeroVector;
	}
	FTransform BodyTransform(FRotator::ZeroRotator, FVector::ZeroVector, Scale);
	UpdateInstanceWorldLocation(Index, BodyTransform, 0);
}


FTransform AVehicleInstancer::GetWorldTransformFromLocal(const FTransform& InParentTransform, const FQuat& InLocalRotator, const FVector& InLocalLocation, const FVector& InLocalScale)
{
	FQuat Rotator = InParentTransform.TransformRotation(InLocalRotator);
	FVector Location = InParentTransform.TransformPosition(InLocalLocation);
	FVector Scale = InParentTransform.GetScale3D();
	return { Rotator, Location, Scale };
}


#if WITH_EDITOR


void AVehicleInstancer::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	DummyMesh->Modify();
	if (bShowMeasureTool)
	{
		DummyMesh->SetWorldTransform(OriginTransform);
		ToolComponent->Modify();
		ToolComponent->LineEndTransform = GetTransform();
		ToolComponent->MarkRenderStateDirty();

		float Distance = (ToolComponent->LineEndTransform.GetLocation() - ToolComponent->LineStartTransform.GetLocation()).Size();
		TCHAR Buff[20] = {};
		FCString::Snprintf(Buff, sizeof(Buff), TEXT("%f"), Distance);
		FPlatformApplicationMisc::ClipboardCopy(Buff);
		GLog->Log(FString::Printf(TEXT("Copied \"%s\" to Clipboard"), Buff));
	}
	else
	{
		DummyMesh->SetRelativeLocation(FVector::ZeroVector);
	}



}


void AVehicleInstancer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (FVehicleWheelMesh& Wheel : WheelMeshes)
	{
		if (Wheel.WheelMesh)
		{
			Wheel.WheelMesh->PostEditChangeProperty(PropertyChangedEvent);
		}
	}

	if (PropertyChangedEvent.GetPropertyName().ToString().Equals(TEXT("bShowMeasureTool")))
	{
		GLog->Log(PropertyChangedEvent.GetPropertyName().ToString());
		GLog->Log(FString::FromInt(bShowMeasureTool));
		Modify();
		DummyMesh->Modify();
		if (bShowMeasureTool)
		{
			DummyMesh->SetStaticMesh(BodyMesh->GetStaticMesh());
			OriginTransform = GetTransform();
		}
		else
		{
			DummyMesh->SetStaticMesh(nullptr);
			DummyMesh->SetRelativeLocation(FVector::ZeroVector);
		}

		ToolComponent->Modify();
		ToolComponent->bShowMeasureTool = bShowMeasureTool;
		ToolComponent->LineStartTransform = OriginTransform;
		ToolComponent->LineEndTransform = OriginTransform;
		ToolComponent->MarkRenderStateDirty();


	}

}

#endif