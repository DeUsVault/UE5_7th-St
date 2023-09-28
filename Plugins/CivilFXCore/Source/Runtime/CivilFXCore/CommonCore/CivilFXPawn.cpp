// Fill out your copyright notice in the Description page of Project Settings.


#include "CivilFXPawn.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/CollisionProfile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/InputSettings.h"
#include "Engine/Engine.h"
#include "GameFramework/TouchInterface.h"

#include "UObject/UObjectIterator.h"
#include "Components/Widget.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SceneManagement.h"

#include "CameraDirector.h"

#include "Engine/World.h"

// Sets default values
ACivilFXPawn::ACivilFXPawn(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

	// Default to no tick function, but if we set 'never ticks' to false (so there is a tick function) it is enabled by default
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.SetTickFunctionEnable(false);


	//Set up collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComponent->InitSphereRadius(35.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = false;
	CollisionComponent->bTraceComplexOnMove = true;
	//CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = CollisionComponent;

	//Set up movement component
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComponent->UpdatedComponent = CollisionComponent;
	//floating pawn properties
	UFloatingPawnMovement* FloatingMovement = Cast<UFloatingPawnMovement>(MovementComponent);

	/** See UFloatingPawnMovement */
	PawnMaxSpeed = 1200.f;
	PawnAccel = 4000.f;
	PawnDecel = 8000.f;

	FloatingMovement->MaxSpeed = PawnMaxSpeed * 5;
	FloatingMovement->Acceleration = PawnAccel * 2;
	FloatingMovement->Deceleration = PawnDecel * 2;
	BaseEyeHeight = 0;

	XTranslateRate = 0;
	YTranslateRate = 0;

	XTouchRotateRate = 0;
	YTouchRotateRate = 0;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	SetActorEnableCollision(true);
}

// Called when the game starts or when spawned
void ACivilFXPawn::BeginPlay()
{
	Super::BeginPlay();

	check(Controller);
	PlayerController = CastChecked<APlayerController>(Controller);
	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
	PlayerController->CreateTouchInterface();
	PlayerController->bEnableTouchEvents = true;
	PlayerController->bEnableTouchOverEvents = true;

	/*
		Fixed the mouse on UI inconsistency but cursor will hide on mouse capture
		TODO: maybe not hide cursor
	*/
	FInputModeGameAndUI Mode;
	PlayerController->SetInputMode(Mode);

	PlayerCamera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

}

static float ResolveRate(float, float);



void ACivilFXPawn::OnBeginCameraMovement(const TArray<FVector>& InLocations, const TArray<FRotator>& InRotations, float Duration, float HeightOffset, bool bPreview/*=false*/, bool bLoop/*=true*/)
{
	if (CameraDirector)
	{
		CameraDirector->Destroy(false, false);
		CameraDirector = nullptr;
	}
	if (InLocations.Num() <= 1)
	{
		return;
	}

	CameraDirector = GetWorld()->SpawnActor<ACameraDirector>();
	check(CameraDirector);
	CameraDirector->OnBeginCameraDirectorMove(InLocations, InRotations, Duration, HeightOffset, bPreview, bLoop);

	if (!bCameraMovementStarted && !bPreview)
	{
		bCameraMovementStarted = true;
		RequestEnableTick();
	}
}


void ACivilFXPawn::OnBeginCameraMovement(const FVector& NewLocation, const FRotator& NewRotation, float Duration/*=2.0f*/)
{
	if (Duration <= 0)
	{
		SetActorLocation(NewLocation);
		PlayerController->SetControlRotation(NewRotation);
		return;
	}

	OnBeginCameraMovement({GetActorLocation(), NewLocation}, {PlayerController->GetControlRotation(), NewRotation}, Duration, 0, false, false);

}

void ACivilFXPawn::OnBeginPreviewAtTimeline(float Timeline)
{
	if (CameraDirector == nullptr)
	{
		return;
	}
	FVector Location;
	FRotator Rotation;
	CameraDirector->GetViewAtTimeline(Timeline, Location, Rotation);

	SetActorLocation(Location);
	PlayerController->SetControlRotation(Rotation);

}

void ACivilFXPawn::OnEndCameraMovement()
{
	if (bCameraMovementStarted)
	{
		bCameraMovementStarted = false;
		CameraDirector->Destroy();
		RequestDisableTick();
	}
}

void ACivilFXPawn::OnBeginCreateOrbitObject(TSubclassOf<AActor> ObjectToCreate, const FVector& DefaultLocation)
{
	bDrawingOrbitObject = true;
	bDrawingOrbitObjectLocation = true;
	GLog->Log(FString::Printf(TEXT("OnBeginCreateOrbitObject: %s"), *DefaultLocation.ToString()));
	OrbitObject = GetWorld()->SpawnActor<AActor>(ObjectToCreate, DefaultLocation, FRotator::ZeroRotator);
	RequestEnableTick();
}

void ACivilFXPawn::OnEndCreateOrbitObject()
{
	if (bDrawingOrbitObject)
	{
		bDrawingOrbitObject = false;
		bDrawingOrbitObjectLocation = false;
		RequestDisableTick();
		OrbitObject->Destroy(false, false);
	}
}

FVector ACivilFXPawn::GetOrbitObjectLocation() const
{
	return OrbitObject->GetActorLocation();
}

void ACivilFXPawn::DrawOrbitObject()
{

	if (bDrawingOrbitObject)
	{
		FVector CameraLocation = PlayerCamera->GetCameraLocation();
		FVector ObjectLocation;
		if (bDrawingOrbitObjectLocation)
		{
			ObjectLocation = CameraLocation + (GetActorForwardVector() * 3000.0f);
			OrbitObject->SetActorLocation(ObjectLocation);
		}
		else
		{
			ObjectLocation = OrbitObject->GetActorLocation();
		}
		DrawDebugLine(GetWorld(), ObjectLocation, ObjectLocation + FVector::DownVector * WORLD_MAX, FColor::Blue);

		for (int32 i = 0; i < OrbitViewLocations.Num() - 1; ++i)
		{
			DrawDebugLine(GetWorld(), OrbitViewLocations[i], OrbitViewLocations[i + 1], FColor::Yellow);
			//GLog->Log(FString::Printf(TEXT("OrbitViewLocations[i]: %s"), *OrbitViewLocations[i].ToString()));
		}

	}
}

void ACivilFXPawn::UpdateOrbitRadiusAndHeight(float InHeight, float InRadius)
{
	OrbitHeight = InHeight;
	OrbitRadius = InRadius;
}

void ACivilFXPawn::UpdateOrbitViewLocations(const TArray<FVector>& InLocations)
{
	OrbitViewLocations = InLocations;
}

void ACivilFXPawn::RequestEnableTick()
{
	if (TickRequestCount == 0)
	{
		SetActorTickEnabled(true);
		DebugTime = 0;
	}
	++TickRequestCount;
}

void ACivilFXPawn::RequestDisableTick()
{
	--TickRequestCount;
	if (TickRequestCount <= 0)
	{
		GLog->Log(FString::Printf(TEXT("%f"), DebugTime));
		SetActorTickEnabled(false);
		TickRequestCount = 0;
	}
}

void ACivilFXPawn::HandleCameraMovement(float DeltaTime)
{
	if (bCameraMovementStarted)
	{
		FVector Location = CameraDirector->GetActorLocation();
		FRotator Rotation = CameraDirector->GetActorRotation();
		/*
		SetActorLocation(Location);
		PlayerController->SetControlRotation(Rotation);

		return;
		*/
		const float LInterpSpeed = 5.f;
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), Location, DeltaTime, LInterpSpeed));
		PlayerController->SetControlRotation(FMath::RInterpTo(PlayerController->GetControlRotation(), Rotation, DeltaTime, LInterpSpeed));
	}
}

void ACivilFXPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GLog->Log(FString::Printf(TEXT("Pawn Ticking - Request Count: %d"), TickRequestCount));
	HandleCameraMovement(DeltaTime);
	HandleTouchInput();
	DrawOrbitObject();
}

UPawnMovementComponent* ACivilFXPawn::GetMovementComponent() const
{
	return MovementComponent;
}

static void InitializeCFXPawnInputBindings()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_MoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_MoveForward", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_MoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_MoveRight", EKeys::D, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_MoveUp", EKeys::E, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_MoveUp", EKeys::Q, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_Turn", EKeys::MouseX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("_LookUp", EKeys::MouseY, -1.f));


		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_LMouseInput", EKeys::LeftMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_RMouseInput", EKeys::RightMouseButton));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_LMouseDoubleInput", EKeys::LeftMouseButton));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_MMouseInput", EKeys::MiddleMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_SUMouseInput", EKeys::MouseScrollUp));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_SDMouseInput", EKeys::MouseScrollDown));


		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("L_Shift", EKeys::LeftShift));

		//Added for UI control
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Delete", EKeys::Delete));

		//Quit game
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_Escape", EKeys::Escape));


		//for testing only
		//remove later
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_TestCreating", EKeys::One));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("_TestReleasing", EKeys::Two));
	}
}

// Called to bind functionality to input
void ACivilFXPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	InitializeCFXPawnInputBindings();

	PlayerInputComponent->BindAxis("_MoveForward", this, &ACivilFXPawn::MoveForward);
	PlayerInputComponent->BindAxis("_MoveRight", this, &ACivilFXPawn::MoveRight);
	PlayerInputComponent->BindAxis("_MoveUp", this, &ACivilFXPawn::MoveUp_World);

	PlayerInputComponent->BindAxis("_Turn", this, &ACivilFXPawn::TurnAtRate);
	PlayerInputComponent->BindAxis("_LookUp", this, &ACivilFXPawn::LookUpAtRate);

	PlayerInputComponent->BindAction("_LMouseInput", IE_Pressed, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_LMouseInput", IE_Released, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_RMouseInput", IE_Pressed, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_RMouseInput", IE_Released, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_MMouseInput", IE_Pressed, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_MMouseInput", IE_Released, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_SUMouseInput", IE_Released, this, &ACivilFXPawn::HandleMouseInput);
	PlayerInputComponent->BindAction("_SDMouseInput", IE_Released, this, &ACivilFXPawn::HandleMouseInput);



	PlayerInputComponent->BindAction("_LMouseInput", IE_Pressed, this, &ACivilFXPawn::HandleLeftMouseUpNotOverUI);
	PlayerInputComponent->BindAction("_LMouseDoubleInput", IE_DoubleClick, this, &ACivilFXPawn::HandleLeftMouseDoubleClickInput);

	PlayerInputComponent->BindAction("L_Shift", IE_Pressed, this, &ACivilFXPawn::HandleLeftShift);
	PlayerInputComponent->BindAction("L_Shift", IE_Released, this, &ACivilFXPawn::HandleLeftShift);


	//touch
	PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACivilFXPawn::BeginTouch);
	PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACivilFXPawn::EndTouch);
	PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ACivilFXPawn::TouchUpdate);

	//quit
	PlayerInputComponent->BindAction("_Escape", IE_Released, this, &ACivilFXPawn::EndApplication);

	//test binding
	PlayerInputComponent->BindAction("_TestCreating", EInputEvent::IE_Pressed, this, &ACivilFXPawn::TestCreatingVehicle);
	PlayerInputComponent->BindAction("_TestReleasing", EInputEvent::IE_Pressed, this, &ACivilFXPawn::TestReleasingVehicle);

}


void ACivilFXPawn::TestCreatingVehicle()
{


	//Disable setview target
//PlayerController->PlayerCameraManager->EndPlay(EEndPlayReason::EndPlayInEditor);
	FTViewTarget ViewTarget;
	FVector Location;
	FRotator Rotation;
	//PlayerController->PlayerCameraManager->GetActorEyesViewPoint(Location, Rotation);
	//PlayerController->GetActorEyesViewPoint(Location, Rotation);
	//PlayerController->GetPlayerViewPoint(Location, Rotation);

	//SetActorTransform(FTransform(Rotation, Location));
	//PlayerController->SetViewTarget(nullptr);
	//PlayerController->PlayerCameraManager->StopAllCameraAnims(true);
	//PlayerController->PlayerCameraManager->EndViewTarget(PlayerController);
	//PlayerController->PlayerCameraManager->PendingViewTarget.Target = NULL;
	//PlayerController->PlayerCameraManager->GetCameraLocation();
	//PlayerController->PlayerCameraManager->GetCameraViewPoint(Location, Rotation);

	Location = PlayerController->PlayerCameraManager->GetCameraLocation();
	Rotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	//BecomeViewTarget(PlayerController);
	const auto& POV = PlayerController->PlayerCameraManager->GetCameraCachePOV();
	PlayerController->PlayerCameraManager->GetCameraViewPoint(Location, Rotation);
	SetActorLocationAndRotation(Location, Rotation);
	PlayerController->SetViewTargetWithBlend(this, 0.1);
	//SetActorLocationAndRotation(Location, Rotation);


	GLog->Log(Location.ToString());
	GLog->Log(Rotation.ToString());


}

void ACivilFXPawn::TestReleasingVehicle()
{

}

bool ACivilFXPawn::IsMouseCursorOnWidget()
{
	//https://answers.unrealengine.com/questions/294890/trouble-with-input-focus-ui-vs-game.html
	// @todo TObjectIterator is not nice
	for (TObjectIterator<UWidget> WidgetItr; WidgetItr; ++WidgetItr)
	{
		TSharedPtr<SWidget> SafeWidget = WidgetItr->GetCachedWidget();

		if (SafeWidget.IsValid() && SafeWidget->IsDirectlyHovered())
		{
			return true;
		}
	}

	return false;
}

#pragma region Mouse Input Handlers
void ACivilFXPawn::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			//GEngine->AddOnScreenDebugMessage(9990, 5.f, FColor::Blue, FString::Printf(TEXT("Move Forward: %f"), Val));
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it

			if(PlayerController->PlayerCameraManager->IsOrthographic())
				AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Z), Val);

			else
				AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);
		}
	}
}
void ACivilFXPawn::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		if (Controller)
		{
			//GEngine->AddOnScreenDebugMessage(9991, 5.f, FColor::Green, FString::Printf(TEXT("Move Right: %f"), Val));
			FRotator const ControlSpaceRot = Controller->GetControlRotation();
			// transform to world space and add it
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val);
		}
	}
}
void ACivilFXPawn::MoveUp_World(float Val)
{
	if (Val != 0.f)
	{
		//GEngine->AddOnScreenDebugMessage(9992, 5.f, FColor::Green, FString::Printf(TEXT("Move Up: %f"), Val));
		AddMovementInput(FVector::UpVector, Val);
	}
}

static float ResolveRate(float PreviousRate, float CurrentRate)
{
	float Result = 0;
	if (CurrentRate != 0)
	{
		if (PreviousRate >= 0 && CurrentRate > 0)
		{
			Result = PreviousRate > CurrentRate ? PreviousRate : CurrentRate;
		}
		else if (PreviousRate >= 0 && CurrentRate < 0)
		{
			Result = CurrentRate;
		}
		else if (PreviousRate <= 0 && CurrentRate < 0)
		{
			Result = PreviousRate < CurrentRate ? PreviousRate : CurrentRate;
		}
		else if (PreviousRate <= 0 && CurrentRate > 0)
		{
			Result = CurrentRate;
		}
	}
	else
	{
		Result = PreviousRate;
	}
	return Result;
}

void ACivilFXPawn::TurnAtRate(float Rate)
{
	if (!MouseInputMode || IsTouchEvent())
	{
		return;
	}

	if (MouseInputMode & FMouseInputMask::TRANSLATE)
	{
		XTranslateRate = ResolveRate(XTranslateRate, Rate);
		MoveRight(XTranslateRate);
	}
	if (MouseInputMode & FMouseInputMask::ROTATE)
	{
		AddControllerYawInput(Rate);
		PlayerController->SetMouseLocation(CursorPosition.X, CursorPosition.Y);
	}

	//zoomin / zoomout is already handled
}

void ACivilFXPawn::LookUpAtRate(float Rate)
{
	if (!MouseInputMode || IsTouchEvent())
	{
		return;
	}

	if (MouseInputMode & FMouseInputMask::TRANSLATE)
	{
		YTranslateRate = ResolveRate(YTranslateRate, Rate);
		MoveUp_World(-YTranslateRate);
	}

	if (MouseInputMode & FMouseInputMask::ROTATE)
	{
		AddControllerPitchInput(Rate);
		PlayerController->SetMouseLocation(CursorPosition.X, CursorPosition.Y);
	}

	if (MouseInputMode & FMouseInputMask::ZOOM_IN)
	{
		MoveForward(1.0f);
	}
	else if (MouseInputMode & FMouseInputMask::ZOOM_OUT)
	{
		MoveForward(-1.0f);
	}

}

void ACivilFXPawn::HandleLeftShift()
{
	UFloatingPawnMovement* FloatingMovement = Cast<UFloatingPawnMovement>(MovementComponent);
	if (PlayerController->IsInputKeyDown(EKeys::LeftShift))
	{
		//pressed
		FloatingMovement->MaxSpeed = PawnMaxSpeed * 50;
		FloatingMovement->Acceleration = PawnAccel * 20;
		FloatingMovement->Deceleration = PawnDecel * 20;
	}
	else
	{
		//released
		FloatingMovement->MaxSpeed = PawnMaxSpeed * 5;
		FloatingMovement->Acceleration = PawnAccel * 2;
		FloatingMovement->Deceleration = PawnDecel * 2;
		FloatingMovement->StopMovementImmediately();
	}
}

void ACivilFXPawn::HandleMouseInput()
{

	if (IsMouseCursorOnWidget())
	{
		return;
	}
	else
	{
		OnEndCameraMovement();
	}

	bool bLMousePressed = PlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
	bool bRMousePressed = PlayerController->IsInputKeyDown(EKeys::RightMouseButton);
	
	bool bMMousePressed = PlayerController->IsInputKeyDown(EKeys::MiddleMouseButton);
	bool bScrollUp = PlayerController->IsInputKeyDown(EKeys::MouseScrollUp);
	bool bScrollDown = PlayerController->IsInputKeyDown(EKeys::MouseScrollDown);


	/**  */

	if (bLMousePressed && bRMousePressed)
	{
		if (MouseInputMode & FMouseInputMask::TRANSLATE)
		{
			MouseInputMode |= FMouseInputMask::ZOOM_IN;
		}
		else if (MouseInputMode & FMouseInputMask::ROTATE)
		{
			MouseInputMode |= FMouseInputMask::ZOOM_OUT;
		}
	}
	else if (bLMousePressed && !bRMousePressed)
	{
		MouseInputMode |= FMouseInputMask::TRANSLATE;
		MouseInputMode &= ~FMouseInputMask::ZOOM_IN;
		MouseInputMode &= ~FMouseInputMask::ZOOM_OUT;
	}
	else if (!bLMousePressed && bRMousePressed)
	{
		MouseInputMode |= FMouseInputMask::ROTATE;
		MouseInputMode &= ~FMouseInputMask::ZOOM_IN;
		MouseInputMode &= ~FMouseInputMask::ZOOM_OUT;
	}

	else if (bMMousePressed)
	{
		MouseInputMode |= FMouseInputMask::TRANSLATE;
	}

	else if (bScrollUp && PlayerController->PlayerCameraManager->IsOrthographic())
	{
		PlayerController->PlayerCameraManager->DefaultOrthoWidth = PlayerController->PlayerCameraManager->DefaultOrthoWidth + 1;
	}

	else
	{
		MouseInputMode = FMouseInputMask::NONE;
		XTranslateRate = YTranslateRate = 0;
	}


	//hide mouse cursor when ONLY rotation
	if (MouseInputMode == FMouseInputMask::ROTATE)
	{
		PlayerController->bShowMouseCursor = false;
		PlayerController->GetMousePosition(CursorPosition.X, CursorPosition.Y);
	}
	else
	{
		PlayerController->bShowMouseCursor = true;
	}
}

void ACivilFXPawn::HandleLeftMouseDoubleClickInput()
{
	bHasDoubleClickBroadcasted = true;
	OnMouseDoubleClicked.Broadcast();
}

void ACivilFXPawn::HandleLeftMouseUpNotOverUI()
{
	bHasDoubleClickBroadcasted = false;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [this]
	{
		if (!bHasDoubleClickBroadcasted)
		{
			OnNotOverUILeftMouseDown.Broadcast();
		}
	}, 0.2f, false);
}

#pragma endregion



void ACivilFXPawn::EndApplication()
{
	TEnumAsByte<EQuitPreference::Type> QuitPreference = EQuitPreference::Quit; 
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, QuitPreference, true);
}


#pragma region Touch Input Handlers
void ACivilFXPawn::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{

	FTouchData TouchData;
	TouchData.FingerIndex = FingerIndex;
	TouchData.CurrentLocation = Location;
	TouchData.OriginalLocation = Location;
	Touches.Add(FingerIndex, TouchData);

	RequestEnableTick();
	/*
	if (Touches.Num() == 1)
	{
		RotateOrigin = Touches[FingerIndex].CurrentLocation;
		Touches[FingerIndex].OriginalLocation = Location;

	}


	if (Touches.Num() == 2)
	{
		PinchAmount = (Touches[(ETouchIndex::Type)0].CurrentLocation - Touches[(ETouchIndex::Type)1].CurrentLocation).Size();
		PanOrigin = (Touches[(ETouchIndex::Type)0].CurrentLocation + Touches[(ETouchIndex::Type)1].CurrentLocation) * 0.5f;
	}
	*/
}

void ACivilFXPawn::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	Touches.Remove(FingerIndex);
	if (Touches.Num() == 0)
	{
		XTouchRotateRate = YTouchRotateRate = 0;
		RequestDisableTick();
	}
}

void ACivilFXPawn::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (Touches.Contains(FingerIndex))
	{
		Touches[FingerIndex].CurrentLocation = Location;
	}
}

void ACivilFXPawn::HandleTouchInput()
{
	int32 Count = Touches.Num();
	if (Count == 1)
	{
		FTouchData TD;

		//get the first element of the map
		//TODO: find a better way to do this
		for (const auto& KP : Touches)
		{
			TD = KP.Value;
			break;
		}

		float MouseX = TD.CurrentLocation.X - TD.OriginalLocation.X;
		float MouseY = TD.CurrentLocation.Y - TD.OriginalLocation.Y;

		XTouchRotateRate = ResolveRate(XTouchRotateRate, MouseX) * 0.2f;
		YTouchRotateRate = ResolveRate(YTouchRotateRate, MouseY) * 0.2f;

		//GLog->Log(FString::Printf(TEXT("X %f, Y %f"), XTouchRotateRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation));
		AddControllerYawInput(XTouchRotateRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
		AddControllerPitchInput(YTouchRotateRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
	}
	else if (Count == 2)
	{
		const FTouchData& TD1 = Touches[ETouchIndex::Touch1];
		const FTouchData& TD2 = Touches[ETouchIndex::Touch2];

		float PinchSeperation = (TD1.CurrentLocation - TD2.CurrentLocation).Size();
		float PinchAmount = (TD1.OriginalLocation - TD2.OriginalLocation).Size();
		float Ratio = (FMath::IsNearlyZero(PinchAmount) ? 1.0f : PinchSeperation / PinchAmount) - 1.0f;
		FVector PanOrigin = (TD1.OriginalLocation + TD2.OriginalLocation) * 0.5f;
		//
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Ratio: %f"), Ratio));
		MoveForward(Ratio);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Touch Control")));
		FVector PanOffset = (TD1.CurrentLocation + TD2.CurrentLocation) * 0.5f;
		PanOffset = (PanOffset - PanOrigin) * 0.01f;

		MoveRight(PanOffset.X);
		MoveUp_World(-PanOffset.Y);
	}
}

bool ACivilFXPawn::IsTouchEvent() const
{
	return Touches.Num() > 0;
}

#pragma endregion