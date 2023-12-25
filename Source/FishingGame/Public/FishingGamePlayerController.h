// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FishingGamePlayerController.generated.h"

UCLASS()
class AFishingGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFishingGamePlayerController();

	void RemoveCastingWidget();

	FORCEINLINE float GetCastingProgress() const { return CastingProgress; }

	FORCEINLINE bool GetIsFishing() const { return bFishing; }

	FORCEINLINE bool GetInTransition() const { return bTransition; }

	FORCEINLINE void SetCastingProgress(float Value) { CastingProgress = Value; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|Camera Controls")
	float CameraRotateSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|Camera Controls")
	float ZoomingSpeed = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTransition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bFishing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bFail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bReadyToFish;

	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|Settings")
	float CastingSpeed = 0.5f;

	UPROPERTY(BlueprintReadOnly)
	float CastingProgress = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|UI")
	TSubclassOf<UUserWidget> CastingBarWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|UI")
	TSubclassOf<UUserWidget> ControlsWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|UI")
	TSubclassOf<UUserWidget> PauseWidgetClass;

	UUserWidget* CastingBarWidget;
	UUserWidget* ControlsWidget;
	UUserWidget* PauseWidget;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void PlayerTick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void MoveToMouseCursor();

	void RotateCamera(float Value);
	
	void ZoomCamera(float Value);

	void SetNewMoveDestination(const FVector DestLocation);

	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void ReadyThrowCast();

	void ThrowCast();

	void TogglePauseMenu();

	bool CheckIsFishing() const;
};


