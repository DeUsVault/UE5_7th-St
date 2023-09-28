// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StillCameraViewItem.h"
#include "StillCameraView.generated.h"

class UCameraHierarchyRoot;
class UTreeView;

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UStillCameraView : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void AddItemData(const FCameraViewInfo& InView);
	TArray<FCameraViewInfo> GetCameraViews() const;
	TArray<FString> GetRootsName() const;

	void RequestRemoveItem(UCameraHierarchyModel* Model);

	UFUNCTION(BlueprintCallable)
	void SetIsEditDragDrop(bool bInIsEditDragDrop);
	bool GetIsEditDragDrop() const { return bIsEditDragDrop; }

protected:
	UPROPERTY(meta = (BindWidget))
	UTreeView* TreeViewContainer;
	
	UFUNCTION()
	void HandleGetItemChildren(UObject* Item, TArray<UObject*>& OutChildren);

private:
	UPROPERTY()
	TArray<UCameraHierarchyRoot*> Roots;

	bool bIsEditDragDrop;
};
