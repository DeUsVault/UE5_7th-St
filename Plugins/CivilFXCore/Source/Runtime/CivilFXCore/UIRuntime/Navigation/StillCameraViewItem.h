// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/DragDropOperation.h"
#include "StillCameraViewItem.generated.h"

class UMainMenu;

enum class EItemDropZone:int32;

USTRUCT(BlueprintType)
struct FCameraViewInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString CameraName;
	UPROPERTY(BlueprintReadOnly)
	FString CameraCategory;
	UPROPERTY(BlueprintReadOnly)
	FVector Location = FVector(10.f);
	UPROPERTY(BlueprintReadOnly)
	FRotator Rotation = FRotator(10.f);
	UPROPERTY(BlueprintReadOnly)
	float FoV = 90.0f;
};


UCLASS(Abstract)
class UCameraHierarchyModel : public UObject
{
	GENERATED_BODY()
public:
	virtual bool IsRoot() const { return false; }

	/* @returns the widget name to use for the tree item */
	virtual FText GetText() const PURE_VIRTUAL(UCameraHierarchyModel::GetText, return FText::GetEmpty(););

	void AddChild(UCameraHierarchyModel* Model);

	/**
	 * Insert a child into Children array.
	 * @param InModel the model to be inserted.
	 * @param InModelAfter the model that the InModel is inserted after. (This model must already be in children array if supplied)
	 */
	void InsertChild(UCameraHierarchyModel* InModel, UCameraHierarchyModel* InModelAfter = nullptr);
	void RemoveChild(UCameraHierarchyModel* Model);
	void MoveChildToIndex(UCameraHierarchyModel* InModel, int32 InIndex);

	template<typename T=UObject>
	void GetAllChildren(TArray<T*>& OutChildren) const
	{
		OutChildren.Reset();
		for (UObject* Child : Children)
		{
			OutChildren.AddUnique(CastChecked<T>(Child));
		}
	}

	UCameraHierarchyModel* GetParent() const;
	bool IsParentOf(UCameraHierarchyModel* Model) const;
	void ReorderChildren(UCameraHierarchyModel* DraggedChild, UCameraHierarchyModel* DroppedChild);

	void SetOuterOwner(UObject* InOwner);
	UObject* GetOuterOwner() const;

	//DragDrop
	virtual TOptional<EItemDropZone> HandleCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, UCameraHierarchyModel* TargetItem);
	//~

protected:
	UPROPERTY()
	TArray<UCameraHierarchyModel*> Children;
	UPROPERTY()
	UCameraHierarchyModel* Parent;

	UPROPERTY(Transient)
	TWeakObjectPtr<UObject> OuterOwner;
};

UCLASS()
class UCameraHierarchyRoot : public UCameraHierarchyModel
{
	GENERATED_BODY()
public:
	virtual bool IsRoot() const { return true; }
	virtual FText GetText() const override { return RootText; }


	bool IsSame(const FString& InRootName) { return RootText.ToString().Equals(InRootName); };
	void SetRootText(const FString& InRootName) { RootText = FText::FromString(InRootName); }

private:
	FText RootText;
};

UCLASS(BlueprintType)
class UStillCameraHierarchyWidget : public UCameraHierarchyModel
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FCameraViewInfo CameraView;

	UPROPERTY(BlueprintReadOnly)
	int32 Id = -1;

	virtual FText GetText() const override { return FText::FromString(CameraView.CameraName); }
	
	static UStillCameraHierarchyWidget* MakeObject(const FCameraViewInfo& InView, int32 Id);
};

//==

class UStillCameraView;
/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UStillCameraViewItem : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly)
	UCameraHierarchyModel* Model;

	UPROPERTY(BlueprintReadOnly)
	UMainMenu* MainMenu;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View Item Settings")
	FSlateBrush ExpandedNormal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View Item Settings")
	FSlateBrush ExpandedHover;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View Item Settings")
	FSlateBrush CollapsedNormal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View Item Settings")
	FSlateBrush CollapsedHover;

	//IUserObjectListEntry interfaces
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	//~
	
	//DragDrop
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//~

	UFUNCTION(BlueprintCallable)
	bool IsRoot() const;

	UFUNCTION(BlueprintCallable)
	void HandleMainButtonClicked();
	UFUNCTION(BlueprintCallable)
	void HandleDeleteButtonClicked();
	UFUNCTION(BlueprintCallable)
	void HandleExpandButtonClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_RefreshViewItem(const FText& InItemText, bool bIsRoot, bool bIsEditDragDrop);

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	TOptional<EItemDropZone> ItemDropZone;
	UStillCameraView* GetOuterOwnerView() const;

};