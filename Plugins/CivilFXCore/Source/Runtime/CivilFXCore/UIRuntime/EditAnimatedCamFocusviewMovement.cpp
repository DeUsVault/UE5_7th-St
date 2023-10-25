// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "EditAnimatedCamFocusviewMovement.h"
#include "CivilFXCore/CommonCore/CivilFXPawn.h"
#include "EditAnimatedCameraPanel.h"
#include "EditAnimatedCameraOverviewButton.h"
#include "TextableButton.h"

#include "CivilFXCore/TrafficSim/TrafficPath.h" //for FSplineBuilder
#include "ChevronCreator.h"

#include "Components/WidgetSwitcher.h"
#include "Components/WidgetSwitcherSlot.h"
#include "Components/Image.h"
#include "Components/SpinBox.h"
#include "Components/Button.h"
#include "Components/MenuAnchor.h"
#include "IntSpinBox.h"
//#include "Layout/Geometry.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "DrawDebugHelpers.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/SceneCapture2D.h"
#include "SceneView.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include "Kismet/KismetMathLibrary.h"

#include "Math/NumericLimits.h"
#include "CollisionDebugDrawingPublic.h"

void UEditAnimatedCamFocusviewMovement::OnPanelVisible()
{
    TempLocations = CameraNodeData->Locations;
    TempRotations = CameraNodeData->Rotations;

    UpdateVisualizer();

    bAddNewObject = false;

    HeightSpinBox->SetValue(CameraNodeData->Height);
    TiltAngleSpinBox->SetValue(CameraNodeData->XRadius);
    ChevronSpeedSpinBox->SetValue(CameraNodeData->YRadius);
    ChevronSizeSpinBox->SetValue(CameraNodeData->Size);

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEditAnimatedCamFocusviewMovement::UpdateProjectedLines, 0.02f, true);

    //Read GeneralNotes.txt for explanation
    ColorButton->WidgetStyle.Normal.TintColor = FSlateColor(FLinearColor(CameraNodeData->CenterLocation));
    ColorButton->WidgetStyle.Hovered.TintColor = FSlateColor(FLinearColor(CameraNodeData->CenterLocation));
    ColorButton->WidgetStyle.Pressed.TintColor = FSlateColor(FLinearColor(CameraNodeData->CenterLocation));
}

void UEditAnimatedCamFocusviewMovement::OnPanelHidden()
{
    if (Chevron)
    {
        Chevron->Destroy(false, false);
        Chevron = nullptr;
    }

    for (AActor* Object : MovementObjects)
    {
        Object->Destroy(false, false);
    }
    MovementObjects.Empty();

    for (AActor* Object : ArrowObjects)
    {
        Object->Destroy(false, false);
    }
    ArrowObjects.Empty();

    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UEditAnimatedCamFocusviewMovement::NativeConstruct()
{
    Super::NativeConstruct();


    //TArray<UUserWidget*> Widgets;
	//UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UEditAnimatedCameraPanel::StaticClass());
    //PanelWithRenderWidgetViewport = Cast<UEditAnimatedCameraPanel>(Widgets[0]);

    ParentSwitcher = (UWidgetSwitcher*)Slot->Parent;

    //check(PanelWithRenderWidgetViewport);
    check(ParentSwitcher);

    PlayerPawn->OnMouseDoubleClicked.AddUObject(this, &UEditAnimatedCamFocusviewMovement::OnLeftMouseDoubleCicked);
    PlayerPawn->OnNotOverUILeftMouseDown.AddUObject(this, &UEditAnimatedCamFocusviewMovement::OnNotOverUILeftMouseDown);

    GenerateViewButton->GetButton()->OnClicked.AddDynamic(this, &UEditAnimatedCamFocusviewMovement::OnGenerateViewButtonClicked);

    ChevronSpeedSpinBox->OnValueCommitted.AddDynamic(this, &UEditAnimatedCamFocusviewMovement::HandleChevronSpeedValueComitted);
    ChevronSizeSpinBox->OnValueCommitted.AddDynamic(this, &UEditAnimatedCamFocusviewMovement::HandleChevronSizeValueCommitted);
    MenuAnchor->OnMenuOpenChanged.AddDynamic(this, &UEditAnimatedCamFocusviewMovement::HandleMenuAnchorOpenChanged);
}

void UEditAnimatedCamFocusviewMovement::NativeDestruct()
{
    PlayerPawn->OnMouseDoubleClicked.RemoveAll(this);
    PlayerPawn->OnNotOverUILeftMouseDown.RemoveAll(this);
    OnPanelHidden();
    Super::NativeDestruct();
}


void UEditAnimatedCamFocusviewMovement::OnGenerateViewButtonClicked()
{
    GenerateViewButton->OnDeselected();

    int32 ElementCount = TempLocations.Num();
    if (ElementCount <= 1)
    {
        return;
    }

    TArray<FVector> LocalLocations = TempLocations;
    TempRotations.Empty();

    //pad start and end control points
    FVector ControlStart = LocalLocations[0] + (LocalLocations[0] - LocalLocations[1]);
    FVector ControlEnd = LocalLocations[ElementCount - 1] + (LocalLocations[ElementCount - 1] - LocalLocations[ElementCount - 2]);

    LocalLocations.Insert(ControlEnd, ElementCount);
    LocalLocations.Insert(ControlStart, 0);

    FSplineBuilder Spline(LocalLocations);

    for (int32 i = 0; i < ElementCount; ++i)
    {
        const float Time = i == 0 ? 0.0f : (float)i / (ElementCount - 1);

        FVector NewLocation = Spline.GetPointOnPath(Time);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, NewLocation + FVector::UpVector * 100, NewLocation + FVector::DownVector * WORLD_MAX, ECC_WorldStatic);
        NewLocation = HitResult.Location;

        FVector Tangent = Spline.GetTangentOnPath(Time);
        FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(NewLocation, NewLocation + Tangent * 100.f);
        Rotation.SetComponentForAxis(EAxis::Y, TiltAngleSpinBox->GetValue());

        TempLocations[i] = NewLocation;
        TempRotations.Add(Rotation);
    }

    CameraNodeData->Locations = TempLocations;
    CameraNodeData->Rotations = TempRotations;
    CameraNodeData->Height = HeightSpinBox->GetValue();
    CameraNodeData->XRadius = TiltAngleSpinBox->GetValue();

    UpdateVisualizer();

    const FVector ThumbnailLocation = TempLocations[0] + FVector::UpVector * HeightSpinBox->GetValue();
    EditingButton->GenerateThumbnail(ThumbnailLocation, TempRotations[0]);

    //Refresh preview data
    PlayerPawn->OnBeginCameraMovement(CameraNodeData->Locations, CameraNodeData->Rotations, CameraNodeData->Duration, CameraNodeData->Height, true);
}

void UEditAnimatedCamFocusviewMovement::OnLeftMouseDoubleCicked()
{

    if (ParentSwitcher->GetActiveWidget() == this)
    {
        if (EditingButton->bPendingDelete)
        {
            return;
        }
        GLog->Log("Double Click");
        FHitResult HitResult;
        if (GetHitResultUnderCursor(HitResult))
        {
            FVector Location = HitResult.Location;

            //Location.Z += HeightSpinBox->GetValue();
            const float Height = HeightSpinBox->GetValue();

            if (!bAddNewObject && TempLocations.Num() == 1)
            {
                TempLocations[0] = Location;

                MovementObjects[0]->SetActorLocation(Location + FVector::UpVector * Height);

                bAddNewObject = true;
            }
            else
            {
                int32 Index = FindClosetPointInPoints(Location, TempLocations);
                check(Index != -1);
                //Index = Index == 0 ? Index : Index + 1;
                TempLocations.Insert(Location, Index);
                TempRotations.Insert(FRotator::ZeroRotator, Index);

                AActor* Object = GetWorld()->SpawnActor<AActor>(MovementObjectClass, Location + FVector::UpVector * Height, FRotator::ZeroRotator);
                MovementObjects.Add(Object);
            }
        }
    }
}

void UEditAnimatedCamFocusviewMovement::OnNotOverUILeftMouseDown()
{
    if (ParentSwitcher->GetActiveWidget() == this)
    {
        GLog->Log("Single Click");
        FHitResult HitResult;
        if (GetHitResultUnderCursor(HitResult))
        {

        }
    }
}

void UEditAnimatedCamFocusviewMovement::HandleMenuAnchorOpenChanged(bool bIsOpen)
{
    if (!bIsOpen)
    {
        FSlateColor SlateColor = ColorButton->WidgetStyle.Normal.TintColor;
        FVector Color = SlateColor.GetSpecifiedColor();
        GLog->Log(Color.ToString());
        CameraNodeData->CenterLocation = Color;
    }

}

void UEditAnimatedCamFocusviewMovement::HandleChevronSpeedValueComitted(float NewValue, ETextCommit::Type)
{
    CameraNodeData->YRadius = NewValue;
}

void UEditAnimatedCamFocusviewMovement::HandleChevronSizeValueCommitted(int32 NewValue, ETextCommit::Type)
{
    CameraNodeData->Size = NewValue;
}

bool UEditAnimatedCamFocusviewMovement::GetHitResultUnderCursor(FHitResult& OutHitResult)
{
    //get screen mouse position
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    FVector2D MousePosition;
    PC->GetMousePosition(MousePosition.X, MousePosition.Y);

    const FGeometry& CachedGeometry = PanelWithRenderWidgetViewport->RenderTextureViewport->GetCachedGeometry();

    //calculate screen mouse position inside of the render texture viewport
    FVector2D ScreenPos;
    USlateBlueprintLibrary::ScreenToWidgetLocal(GetWorld(), CachedGeometry, MousePosition, ScreenPos);

    UTextureRenderTarget2D* Target2D = PanelWithRenderWidgetViewport->GetSceneCapture2D()->GetCaptureComponent2D()->TextureTarget;
    FVector2D RTViewportSize(Target2D->GetSurfaceWidth(), Target2D->GetSurfaceHeight());

    FVector2D LocalSize = CachedGeometry.GetLocalSize();
    float NewMouseX = FMath::GetMappedRangeValueUnclamped(FVector2D(0, LocalSize.X), FVector2D(0, RTViewportSize.X), ScreenPos.X);
    float NewMouseY = FMath::GetMappedRangeValueUnclamped(FVector2D(0, LocalSize.Y), FVector2D(0, RTViewportSize.Y), ScreenPos.Y);

    //early exit if mapped mouse position is not within the viewport
    if (NewMouseX < 0 && NewMouseX > RTViewportSize.X && NewMouseY < 0 && NewMouseY > RTViewportSize.Y)
    {
        return false;
    }

    FVector WorldOrigin;
    FVector WorldDirection;
    Capture2D_DeProject(PanelWithRenderWidgetViewport->GetSceneCapture2D(), FVector2D(NewMouseX, NewMouseY), WorldOrigin, WorldDirection);

    FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), true, PlayerPawn);
    if (GetWorld()->LineTraceSingleByChannel(OutHitResult, WorldOrigin, WorldOrigin + WorldDirection * WORLD_MAX, ECC_WorldStatic, CollisionQueryParams))
    {
        return true;
    }

    return false;
}

/** https://forums.unrealengine.com/development-discussion/c-gameplay-programming/117296-deproject-screen-to-world-and-scenecapture2d */
void UEditAnimatedCamFocusviewMovement::Capture2D_DeProject(class ASceneCapture2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection)
{
    if (Target)
    {
        CaptureComponent2D_DeProject(Target->GetCaptureComponent2D(), ScreenPos, OutWorldOrigin, OutWorldDirection);
    }
}

void UEditAnimatedCamFocusviewMovement::UpdateVisualizer()
{
  
    /** Remove everything */

    if (Chevron)
    {
        Chevron->Destroy(false, false);
        Chevron = nullptr;
    }

    for (AActor* Object : MovementObjects)
    {
        Object->Destroy(false, false);
    }
    MovementObjects.Empty();

    for (AActor* Object : ArrowObjects)
    {
        Object->Destroy(false, false);
    }
    ArrowObjects.Empty();

    check(TempLocations.Num() == TempRotations.Num());

    //Recreate everything

    Chevron = GetWorld()->SpawnActor<AChevronCreator>();
    Chevron->BuildChevronMesh(CameraNodeData->Locations, FLinearColor(CameraNodeData->CenterLocation), CameraNodeData->YRadius, CameraNodeData->Size);
    Chevron->SetActorHiddenInGame(false);

    const float Height = HeightSpinBox->GetValue();
    for (int32 i = 0; i < TempLocations.Num(); ++i)
    {
        FVector Location = TempLocations[i] + FVector::UpVector * Height;
        FRotator Rotation = TempRotations[i];

        AActor* Object = GetWorld()->SpawnActor<AActor>(MovementObjectClass, Location, Rotation);
        MovementObjects.Add(Object);
    }
    
    for (int32 i = 0; i < TempLocations.Num() - 1; ++i)
    {
        FVector Location = FMath::Lerp(TempLocations[i], TempLocations[i + 1], 0.5f);
        FRotator Rotation = FMath::Lerp(TempRotations[i], TempRotations[i + 1], 0.5f);
        Rotation.SetComponentForAxis(EAxis::Y, 0);
        AActor* ArrowObject = GetWorld()->SpawnActor<AActor>(ArrowObjectClass, Location, Rotation);
        ArrowObjects.Add(ArrowObject);
    }
}

void UEditAnimatedCamFocusviewMovement::UpdateProjectedLines()
{
    for (const AActor* Object : MovementObjects)
    {
        FVector Location = Object->GetActorLocation();
        TArray<FHitResult> HitResults;
        //DrawLineTraces(GetWorld(), Location, Location + FVector::DownVector * WORLD_MAX, HitResults, 0.02f);
    }
}

int32 UEditAnimatedCamFocusviewMovement::FindClosetPointInPoints(const FVector& Point, const TArray<FVector>& Points)
{
    TArray<FVector> LocalPoints = Points;
    int32 ElementCount = LocalPoints.Num();

    if (ElementCount == 1)
    {
        return 1;
    }


    //padding
    FVector ControlStart = LocalPoints[0] + (LocalPoints[0] - LocalPoints[1]);
    FVector ControlEnd = LocalPoints[ElementCount - 1] + (LocalPoints[ElementCount - 1] - LocalPoints[ElementCount - 2]);

    LocalPoints.Insert(ControlEnd, ElementCount);
    LocalPoints.Insert(ControlStart, 0);



    float MinDistance = TNumericLimits<float>::Max();
    int32 Index = -1;
    int32 LeftIndex = -1;
    int32 RightIndex = -1;
    for (int32 i = 1; i < LocalPoints.Num() - 1; ++i)
    {
        float CurrentDis = FVector::DistSquared(Point, LocalPoints[i]);
        if (CurrentDis < MinDistance)
        {
            Index = i;
            LeftIndex = i - 1;
            RightIndex = i + 1;
            MinDistance = CurrentDis;
        }
    }

    float LeftDistance = FVector::DistSquared(Point, LocalPoints[LeftIndex]);
    float RightDistance = FVector::DistSquared(Point, LocalPoints[RightIndex]);
    if (LeftDistance <= RightDistance)
    {
        Index = LeftIndex;
    }
    else
    {
        //Index = RightIndex;
    }

    return Index;
}

void UEditAnimatedCamFocusviewMovement::CaptureComponent2D_DeProject(class USceneCaptureComponent2D* Target, const FVector2D& ScreenPos, FVector& OutWorldOrigin, FVector& OutWorldDirection)
{
    if ((Target == nullptr) || (Target->TextureTarget == nullptr))
    {
        return;
    }

    const FTransform& Transform = Target->GetComponentToWorld();
    FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();
    FVector ViewLocation = Transform.GetTranslation();

    // swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
    ViewMatrix = ViewMatrix * FMatrix(
        FPlane(0, 0, 1, 0),
        FPlane(1, 0, 0, 0),
        FPlane(0, 1, 0, 0),
        FPlane(0, 0, 0, 1));

    const float FOV = Target->FOVAngle * (float)PI / 360.0f;

    FIntPoint CaptureSize(Target->TextureTarget->GetSurfaceWidth(), Target->TextureTarget->GetSurfaceHeight());

    float XAxisMultiplier;
    float YAxisMultiplier;

    if (CaptureSize.X > CaptureSize.Y)
    {
        // if the viewport is wider than it is tall
        XAxisMultiplier = 1.0f;
        YAxisMultiplier = CaptureSize.X / (float)CaptureSize.Y;
    }
    else
    {
        // if the viewport is taller than it is wide
        XAxisMultiplier = CaptureSize.Y / (float)CaptureSize.X;
        YAxisMultiplier = 1.0f;
    }

    FMatrix    ProjectionMatrix = FReversedZPerspectiveMatrix(
        FOV,
        FOV,
        XAxisMultiplier,
        YAxisMultiplier,
        GNearClippingPlane,
        GNearClippingPlane
    );

    const FMatrix InverseViewMatrix = ViewMatrix.InverseFast();
    const FMatrix InvProjectionMatrix = ProjectionMatrix.Inverse();

    const FIntRect ViewRect = FIntRect(0, 0, CaptureSize.X, CaptureSize.Y);

    FSceneView::DeprojectScreenToWorld(ScreenPos, ViewRect, InverseViewMatrix, InvProjectionMatrix, OutWorldOrigin, OutWorldDirection);
}
/** ~ */
