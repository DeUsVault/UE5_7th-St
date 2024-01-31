// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/PlayerCameraManager.h"

#include "CivilFXPawn.generated.h"

class USphereComponent;
class UPawnMovementComponent;
class APlayerController;

//for testing
//remove later
struct FVehicleInstanceID;

UCLASS()
class CIVILFXCORE_API ACivilFXPawn : public APawn
{
	GENERATED_BODY()

public:
	ACivilFXPawn(const FObjectInitializer& ObjectInitializer);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Camera Movement for Animated Button */
	void OnBeginCameraMovement(const TArray<FVector>& InLocations, const TArray<FRotator>& InRotations, float Duration, float HeightOffset, bool bPreview=false, bool bLoop=true);
	void OnBeginPreviewAtTimeline(float Timeline);
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Camera")
	class ACameraDirector* CameraDirector;

	/** Camera Movement For Still Button */
	/**
	* Set the view target blending with variable control
	* @param NewLocation - Target Location
	* @param NewRotation - Target Rotation
	* @param Duration - time taken to blend
	* @param BlendFunc - Cubic, Linear etc functions for blending
	* @param BlendExp -  Exponent, used by certain blend functions to control the shape of the curve.
	*/
	UFUNCTION(BlueprintCallable, Category = "Still Camera")
	void OnBeginCameraMovement(const FVector& NewLocation, const FRotator& NewRotation, float Duration=2.0f);
	void OnEndCameraMovement();

	UFUNCTION(BlueprintCallable)
	bool IsMouseCursorOnWidget();

	float DebugTime;

	/** Orbit Panel functionalities*/
	void OnBeginCreateOrbitObject(TSubclassOf<AActor> ObjectToCreate, const FVector& DefaultLocation);
	void OnEndCreateOrbitObject();
	FVector GetOrbitObjectLocation() const;
	void DrawOrbitObject();
	bool bDrawingOrbitObject;
	bool bDrawingOrbitObjectLocation;
	float OrbitRadius;
	float OrbitHeight;
	TArray<FVector> OrbitViewLocations;
	void UpdateOrbitRadiusAndHeight(float InHeight, float InRadius);
	void UpdateOrbitViewLocations(const TArray<FVector>& InLocations);
	AActor* OrbitObject;


	/** Movement Panel functionalities */
	DECLARE_MULTICAST_DELEGATE(FOnMouseDoubleClicked)
	FOnMouseDoubleClicked OnMouseDoubleClicked;
	DECLARE_MULTICAST_DELEGATE(FOnNotOverUILeftMouseDown)
	FOnNotOverUILeftMouseDown OnNotOverUILeftMouseDown;
	bool bHasDoubleClickBroadcasted;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;
	/** DefaultPawn movement component */
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPawnMovementComponent* MovementComponent;

	/**
	* Input callback to move forward in local space (or backward if Val is negative).
	* @param Val Amount of movement in the forward direction (or backward if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void MoveForward(float Val);

	/**
	* Input callback to strafe right in local space (or left if Val is negative).
	* @param Val Amount of movement in the right direction (or left if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void MoveRight(float Val);

	/**
	* Input callback to move up in world space (or down if Val is negative).
	* @param Val Amount of movement in the world up direction (or down if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void MoveUp_World(float Val);

	UFUNCTION(Category = "Pawn")
	virtual void HandleMouseInput();

	//UFUNCTION(Category = "Pawn")
	//virtual void HandleRMouseInput();
	UFUNCTION()
	virtual void HandleLeftMouseDoubleClickInput();
	UFUNCTION()
	virtual void HandleLeftMouseUpNotOverUI();

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void TurnAtRate(float Rate);

	/**
	* Called via input to look up at a given rate (or down if Rate is negative).
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void LookUpAtRate(float Rate);
	
	UFUNCTION()
	virtual void HandleLeftShift();

	UFUNCTION()
	virtual void HandleSpaceBar();

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void EndApplication();

	
	//Touch interface
	struct FTouchData
	{
		ETouchIndex::Type FingerIndex;
		FVector OriginalLocation;
		FVector CurrentLocation;
	};
	TMap<ETouchIndex::Type, FTouchData> Touches;
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	bool IsTouchEvent() const;
	void HandleTouchInput();
	/*Touch Input Properties*/
	float XTouchRotateRate;
	float YTouchRotateRate;
	/********/
	//End Touch interface;

	/*Test*/
	void TestCreatingVehicle();
	void TestReleasingVehicle();

private:
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;

	APlayerCameraManager* PlayerCamera;

	float PawnMaxSpeed;
	float PawnAccel;
	float PawnDecel;



	/*Mouse Input Properties*/
	using FMouseInputMode = uint8;
	struct FMouseInputMask
	{
		static CONSTEXPR uint8 NONE			= 0b00000000;
		static CONSTEXPR uint8 TRANSLATE	= 0b00000010;
		static CONSTEXPR uint8 ROTATE		= 0b00000100;
		static CONSTEXPR uint8 ZOOM_IN		= 0b00001000;
		static CONSTEXPR uint8 ZOOM_OUT		= 0b00010000;
	};

	FMouseInputMode MouseInputMode;
	float XTranslateRate;
	float YTranslateRate;
	FVector2D CursorPosition;
	/********/

	FVector TargetLocation;
	FRotator TargetRotation;
	bool bCameraMovementStarted;
	bool bCameraPaused;
	uint8 TickRequestCount;
	float InterpSpeed;
	void HandleCameraMovement(float DeltaTime);

	void RequestEnableTick();
	void RequestDisableTick();
};
