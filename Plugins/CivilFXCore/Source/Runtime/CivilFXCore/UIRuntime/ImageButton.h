// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomButtonInterface.h"
#include "ImageButton.generated.h"

/**
 * 
 */
UCLASS()
class CIVILFXCORE_API UImageButton : public UUserWidget, public ICustomButtonInterface
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditInstanceOnly, Category="Image Button Settings", meta=(BindWidget,EditCondition="1==2"))
	class UButton* MainButton;

	UPROPERTY(VisibleAnywhere, Category="Image Button Settings", meta=(BindWidget))
	class UImage* MainImage;

	UPROPERTY(EditInstanceOnly, Category = "Overview Button Settings")
	class UMaterial* ParentMat;

	void SetImageResource(UObject* InResourceObject);
	void GenerateThumbnail(const FVector& Location, const FRotator& Rotation);

	virtual void NativeConstruct() override;

	/** ICustomButton Interface */
	virtual TWeakObjectPtr<UButton> GetButton() override {return MainButton;}
	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	/** ~ICustomButton Interface */

private:
	/*Added to fix NativeConstruct is called again when re-adding this to container*/
	bool bConstructed;

};
