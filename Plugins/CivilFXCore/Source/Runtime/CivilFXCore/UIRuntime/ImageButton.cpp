// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "ImageButton.h"
#include "Components/Image.h"

#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

void UImageButton::OnSelected()
{
	ICustomButtonInterface::OnSelected();
	MainButton->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
}

void UImageButton::OnDeselected()
{
	GLog->Log("On Deselected");
	ICustomButtonInterface::OnDeselected();
	MainButton->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::NoDrawType;
}
void UImageButton::SetImageResource(UObject* InResourceObject)
{
	MainImage->SetBrushResourceObject(InResourceObject);
}

void UImageButton::NativeConstruct()
{
	if (bConstructed)
	{
		return;
	}
	bConstructed = true;
	Super::NativeConstruct();
	MainButton->OnClicked.AddDynamic(this, &UImageButton::OnSelected);
}

void UImageButton::GenerateThumbnail(const FVector& Location, const FRotator& Rotation)
{
	ASceneCapture2D* ScenceCapture2D = GetWorld()->SpawnActor<ASceneCapture2D>(Location, Rotation);
	check(ScenceCapture2D);
	USceneCaptureComponent2D* ComponentCapture2D = ScenceCapture2D->GetCaptureComponent2D();
	UTextureRenderTarget2D* TargetTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 150, 100);
	ComponentCapture2D->TextureTarget = TargetTexture;
	ComponentCapture2D->bCaptureEveryFrame = false;
	ComponentCapture2D->bCaptureOnMovement = false;
	ComponentCapture2D->CaptureScene();
	
	UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(ParentMat, this);
	Mat->SetTextureParameterValue(TEXT("Texture"), TargetTexture);
	check(Mat);
	SetImageResource(Mat);

	ScenceCapture2D->Destroy(false, false);
}