// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatedCameraViewItem.h"
#include "../ChevronCreator.h"
#include "CivilFXCore/CommonCore/CivilFXPawn.h"



void UAnimatedCameraHierarchyWidget::Initialize(const FAnimatedCameraNodeData& InCameraData)
{
	AnimatedCameraData = InCameraData;
	bChevronActive = false;

	if (ChevronActor)
	{
		ChevronActor->Destroy(false, false);
		ChevronActor = nullptr;
	}

	if (AnimatedCameraData.IsMovementType())
	{
		//If this asserts, make sure to create this object with a valid outer
		check(GetWorld());
		//Rebuild chevron actor
		ChevronActor = GetWorld()->SpawnActor<AChevronCreator>();
#if WITH_EDITOR
		ChevronActor->SetActorLabel(FString::Printf(TEXT("Chevron %s"), *AnimatedCameraData.CameraName));
#endif
		ChevronActor->BuildChevronMesh(AnimatedCameraData.Locations, FLinearColor(AnimatedCameraData.CenterLocation), AnimatedCameraData.YRadius, AnimatedCameraData.Size);
	}
}

UAnimatedCameraHierarchyWidget* UAnimatedCameraHierarchyWidget::MakeObject(UObject* Outer, const FAnimatedCameraNodeData& InCameraData)
{
	UAnimatedCameraHierarchyWidget* Object = NewObject<UAnimatedCameraHierarchyWidget>(Outer);
	Object->Initialize(InCameraData);

	return Object;
}

FText UAnimatedCameraHierarchyWidget::GetText() const
{
	return FText::FromString(AnimatedCameraData.CameraName);
}

void UAnimatedCameraHierarchyWidget::ToggleChevronMesh()
{
	if (!ChevronActor)
	{
		return;
	}
	bChevronActive = !bChevronActive;
	ChevronActor->SetActorHiddenInGame(!bChevronActive);
}

void UAnimatedCameraHierarchyWidget::StopChevron()
{
	if (ChevronActor)
	{
		bChevronActive = false;
		ChevronActor->SetActorHiddenInGame(!bChevronActive);
	}
}

bool UAnimatedCameraHierarchyWidget::IsMovementType() const
{
	return AnimatedCameraData.IsMovementType();
}

bool UAnimatedCameraHierarchyWidget::IsChevronActive() const
{
	return bChevronActive;
}	



void UAnimatedCameraHierarchyWidget::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (ChevronActor)
	{
		ChevronActor->Destroy(false, false);
		ChevronActor = nullptr;
	}
}

void UAnimatedCameraViewItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	//Can fail if it's a root model
	if (UAnimatedCameraHierarchyWidget* AnimatedModel = Cast<UAnimatedCameraHierarchyWidget>(Model))
	{
		const bool bHasPreview = AnimatedModel->IsMovementType();
		const bool bPreviewActive = AnimatedModel->IsChevronActive();
		BP_RefreshPreview(bHasPreview, bPreviewActive);
	}
}

//==========

void UAnimatedCameraViewItem::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (bIsSelected)
	{
		UAnimatedCameraHierarchyWidget* AnimatedModel = CastChecked<UAnimatedCameraHierarchyWidget>(Model);

		const FAnimatedCameraNodeData& CameraNodeData = AnimatedModel->AnimatedCameraData;
		const float Height = CameraNodeData.CameraType.Equals(TEXT("Movement")) ? CameraNodeData.Height : 0;

		ACivilFXPawn* PlayerPawn = CastChecked<ACivilFXPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		PlayerPawn->OnBeginCameraMovement(CameraNodeData.Locations, CameraNodeData.Rotations, CameraNodeData.Duration, Height);
	}
}

void UAnimatedCameraViewItem::HandlePreviewMeshButtonClicked()
{
	UAnimatedCameraHierarchyWidget* AnimatedModel = CastChecked<UAnimatedCameraHierarchyWidget>(Model);
	AnimatedModel->ToggleChevronMesh();
	const bool bPreviewActive = AnimatedModel->IsChevronActive();
	BP_RefreshPreview(true, bPreviewActive);
}
