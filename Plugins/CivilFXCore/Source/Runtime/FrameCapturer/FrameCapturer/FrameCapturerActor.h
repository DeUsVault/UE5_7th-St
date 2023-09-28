// /** Copyright (C) CivilFX 2020 - All Rights Reserved */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrameCapturerActor.generated.h"

UCLASS()
class FRAMECAPTURER_API AFrameCapturerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFrameCapturerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	class UMediaOutput* MediaOutput;

	UPROPERTY(Transient)
	class UMediaCapture* MediaCapture;


	UPROPERTY(Transient)
	class ASceneCapture2D* SceneCapture2D;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
