// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "StillCameraViewItem.h"
#include "../AnimatedCameraNodeData.h"
#include "AnimatedCameraViewItem.generated.h"


class AChevronCreator;
class UButton;

UCLASS()
class UAnimatedCameraHierarchyWidget : public UCameraHierarchyModel
{
	GENERATED_BODY()
public:
	FAnimatedCameraNodeData AnimatedCameraData;

	virtual FText GetText() const override;
	void Initialize(const FAnimatedCameraNodeData& InCameraData);
	void ToggleChevronMesh();
	void StopChevron();

	bool IsMovementType() const;
	bool IsChevronActive() const;

	static UAnimatedCameraHierarchyWidget* MakeObject(UObject* Outer, const FAnimatedCameraNodeData& InCameraData);
	virtual void BeginDestroy() override;

protected:
	UPROPERTY()
	AChevronCreator* ChevronActor;
	bool bChevronActive;
};


UCLASS()
class CIVILFXCORE_API UAnimatedCameraViewItem : public UStillCameraViewItem
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

protected:
	/** Preview Button Style */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View Item Settings")
	FButtonStyle ActiveStyle;
	/** Preview Button Style */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View Item Settings")
	FButtonStyle DeactiveStyle;
	
	UFUNCTION(BlueprintCallable)
	void HandlePreviewMeshButtonClicked();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RefreshPreview(bool bHasPreview, bool bPreviewActive);
};
