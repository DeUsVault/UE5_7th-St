// /** Copyright (C) CivilFX 2020 - All Rights Reserved */


#include "FrameCapturerActor.h"
#include "fccore.h"
#include "Buffer.h"
#include "CFXMediaOutput.h"
#include "MediaCapture.h"

#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
AFrameCapturerActor::AFrameCapturerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFrameCapturerActor::BeginPlay()
{
	Super::BeginPlay();
	
    fcMP4Config conf;
    conf.video_width = 1920;
    conf.video_height = 1080;
    conf.video_bitrate_mode = fcBitrateMode::VBR;
    conf.video_target_bitrate = 256000;
 
    fcIMP4Context* ctx = fcMP4OSCreateContext(&conf, "F:/CivilFX/UE4 Projects/CFXTypicalSection/Captures/WHF.mp4");
    GLog->Log("AFrameCapturerActor");
    void* test = nullptr;
    fcMP4AddVideoFramePixels(ctx, test, fcPixelFormat_RGBAu8, 10);
    fcReleaseContext(ctx);


    //TSharedPtr<FSceneViewport> Viewport = MakeShareable<FSceneViewport>(GetWorld()->GetGameViewport()->GetGameViewport());
    //MediaOutput->CreateMediaCapture()->CaptureSceneViewport(Viewport, Options);
   
    //UTextureRenderTarget2D* TargetTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1280, 720);
    //MediaOutput->CreateMediaCapture()->CaptureTextureRenderTarget2D(TargetTexture, Options);

    //GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->ViewportClient->EngineShowFlags.Rendering = false;

    MediaOutput = NewObject<UCFXMediaOutput>();
    MediaCapture = MediaOutput->CreateMediaCapture();

    FMediaCaptureOptions Options;
    Options.bResizeSourceBuffer = true;
    MediaCapture->CaptureActiveSceneViewport(Options);
    
    /** Spawn 2d capture component */
    /*
    SceneCapture2D = GetWorld()->SpawnActor<ASceneCapture2D>(FVector::ZeroVector, FRotator::ZeroRotator);
    SceneCapture2D->AttachToActor(GetWorld()->GetFirstPlayerController()->PlayerCameraManager, FAttachmentTransformRules::SnapToTargetIncludingScale);
    
    UTextureRenderTarget2D* TargetTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 150, 100, RTF_RGBA16f);
    SceneCapture2D->GetCaptureComponent2D()->TextureTarget = TargetTexture;

    FMediaCaptureOptions Options;
    Options.bResizeSourceBuffer = true;
    MediaCapture->CaptureTextureRenderTarget2D(TargetTexture, Options);
    */
}

void AFrameCapturerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
    MediaCapture->StopCapture(false);
}

