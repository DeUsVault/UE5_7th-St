// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimatedCameraView.generated.h"

class UTreeView;
class UAnimatedCameraHierarchyWidget;
struct FAnimatedCameraNodeData;

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UAnimatedCameraView : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void StopAllChevron();
	void RedrawView(const TArray<FAnimatedCameraNodeData>& InCameraNodeDatas);

protected:
	UPROPERTY(meta = (BindWidget))
	UTreeView* TreeViewContainer;

	UFUNCTION()
	void HandleGetItemChildren(UObject* Item, TArray<UObject*>& OutChildren);

	/** Cached items to stop chevron */
	UPROPERTY(Transient)
	TArray<UAnimatedCameraHierarchyWidget*> CachedItems;
};
