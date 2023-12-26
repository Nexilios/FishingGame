// Copyright Epic Games, Inc. All Rights Reserved.

#include "FishingGamePlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "FishingGameCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"

AFishingGamePlayerController::AFishingGamePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AFishingGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (CastingBarWidgetClass)
	{
		CastingBarWidget = CreateWidget<UUserWidget>(this, CastingBarWidgetClass);
	}

	if (ControlsWidgetClass)
	{
		ControlsWidget = CreateWidget<UUserWidget>(this, ControlsWidgetClass);
		if (ControlsWidget)
		{
			ControlsWidget->AddToViewport();
		}
	}

	if (PauseWidgetClass)
	{
		PauseWidget = CreateWidget<UUserWidget>(this, PauseWidgetClass);
	}
}

void AFishingGamePlayerController::PlayerTick(float DeltaSeconds)
{
	Super::PlayerTick(DeltaSeconds);

	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}

	if (bReadyToFish)
	{
		CastingProgress = FMath::FInterpConstantTo(CastingProgress, 1.f, DeltaSeconds, CastingSpeed);
	}
}

void AFishingGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AFishingGamePlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AFishingGamePlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("Throw/ReelCast", IE_Pressed, this, &AFishingGamePlayerController::ReadyThrowCast);
	InputComponent->BindAction("Throw/ReelCast", IE_Released, this, &AFishingGamePlayerController::ThrowCast);
	InputComponent->BindAction("PauseGame", IE_Pressed, this, &AFishingGamePlayerController::TogglePauseMenu);
	InputComponent->BindAxis("MoveForward", this, &AFishingGamePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFishingGamePlayerController::MoveRight);
	InputComponent->BindAxis("RotateCamera", this, &AFishingGamePlayerController::RotateCamera);
	InputComponent->BindAxis("ZoomCamera", this, &AFishingGamePlayerController::ZoomCamera);
}

void AFishingGamePlayerController::MoveForward(float Value)
{
	if (Value != 0.0f && !CheckIsFishing())
	{
		AFishingGameCharacter* const PlayerCharacter = Cast<AFishingGameCharacter>(GetCharacter());
		if (PlayerCharacter)
		{
			if (IsFollowingAPath()) StopMovement();

			USpringArmComponent* const CameraBoom = PlayerCharacter->GetCameraBoom();
			if (CameraBoom)
			{
				const FRotator Rotation = CameraBoom->GetRelativeRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				PlayerCharacter->AddMovementInput(Direction, Value);
			}
		}
	}
}

void AFishingGamePlayerController::MoveRight(float Value)
{
	if (Value != 0.0f && !CheckIsFishing())
	{
		AFishingGameCharacter* const PlayerCharacter = Cast<AFishingGameCharacter>(GetCharacter());
		if (PlayerCharacter)
		{
			if (IsFollowingAPath()) StopMovement();

			USpringArmComponent* const CameraBoom = PlayerCharacter->GetCameraBoom();
			if (CameraBoom)
			{
				const FRotator Rotation = CameraBoom->GetRelativeRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				PlayerCharacter->AddMovementInput(Direction, Value);
			}
		}
	}
}

void AFishingGamePlayerController::MoveToMouseCursor()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AFishingGamePlayerController::RotateCamera(float Value)
{
	if (Value != 0.0f)
	{
		AFishingGameCharacter* const PlayerCharacter = Cast<AFishingGameCharacter>(GetCharacter());
		if (PlayerCharacter) {
			USpringArmComponent* const CameraBoom = PlayerCharacter->GetCameraBoom();
			if (CameraBoom)
			{
				const float RotationSpeed = (Value * CameraRotateSpeed) * GetWorld()->GetDeltaSeconds();
				const FRotator CurrentRotation = CameraBoom->GetRelativeRotation();
				const FRotator YawRotation = FRotator(0, RotationSpeed, 0);
				CameraBoom->SetRelativeRotation(CurrentRotation + YawRotation);
			}
		}
	}
}

void AFishingGamePlayerController::ZoomCamera(float Value)
{
	if (Value != 0)
	{
		AFishingGameCharacter* const PlayerCharacter = Cast<AFishingGameCharacter>(GetCharacter());
		if (PlayerCharacter) {
			USpringArmComponent* const CameraBoom = PlayerCharacter->GetCameraBoom();
			if (CameraBoom)
			{
				CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength + (ZoomingSpeed * Value * GetWorld()->GetDeltaSeconds()), 300.f, 1500.f);
			}
		}
	}
}

void AFishingGamePlayerController::ReadyThrowCast()
{
	CastingProgress = 0.f;
	if (bFail) bFail = false;
	if (IsFollowingAPath()) StopMovement();
	if (!bFishing && !bTransition)
	{
		if (CastingBarWidget)
		{
			CastingBarWidget->AddToViewport();
		}
		bReadyToFish = true;

		AFishingGameCharacter* const PlayerCharacter = Cast<AFishingGameCharacter>(GetCharacter());
		if (PlayerCharacter && PlayerCharacter->GetFishMesh() && PlayerCharacter->GetFishMesh()->IsVisible())
		{
			PlayerCharacter->GetFishMesh()->SetVisibility(false);
		}
	}
	else if(bFishing && !bTransition)
	{
		bFishing = false;
		bTransition = true;
	}
}

void AFishingGamePlayerController::ThrowCast()
{
	if (!bFishing && bReadyToFish && !bTransition)
	{
		bReadyToFish = false;
		bTransition = true;
	}
}

bool AFishingGamePlayerController::CheckIsFishing() const
{
	return (bReadyToFish || bFishing || bTransition);
}

void AFishingGamePlayerController::TogglePauseMenu()
{
	if (PauseWidget)
	{
		if (!IsPaused())
		{
			SetPause(true);
			PauseWidget->AddToViewport();
		}
		else
		{
			SetPause(false);
			PauseWidget->RemoveFromParent();
		}
	}
}

void AFishingGamePlayerController::RemoveCastingWidget()
{
	if (CastingBarWidget)
	{
		CastingBarWidget->RemoveFromParent();
	}
}

void AFishingGamePlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	if (APawn* const PlayerPawn = GetPawn())
	{
		float const Distance = FVector::Dist(DestLocation, PlayerPawn->GetActorLocation());

		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AFishingGamePlayerController::OnSetDestinationPressed()
{
	if (!CheckIsFishing())
	{
		bMoveToMouseCursor = true;
	}
}

void AFishingGamePlayerController::OnSetDestinationReleased()
{
	bMoveToMouseCursor = false;
}
