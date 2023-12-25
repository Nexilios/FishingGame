// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FishingGameCharacter.generated.h"

UCLASS(Blueprintable)
class AFishingGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFishingGameCharacter();

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	FORCEINLINE UStaticMeshComponent* GetHookMesh() { return Hook; }

	FORCEINLINE USkeletalMeshComponent* GetFishMesh() { return FishMesh; }

	UFUNCTION(BlueprintCallable)
	void LaunchHook();

	UFUNCTION(BlueprintCallable)
	void ReelHook();

	UFUNCTION(BlueprintCallable)
	void ClearTimersAndVFX();

	UFUNCTION(BlueprintCallable)
	void StartFishing();

	void FishBite();

protected:
	UPROPERTY(EditAnywhere, Category = "FishingGame|Model")
	USkeletalMeshComponent* FishMesh;

	UPROPERTY(EditAnywhere, Category = "FishingGame|Particle")
	class UParticleSystemComponent* FishBiteFXComp;

	UPROPERTY(EditAnywhere, Category = "FishingGame|Hook")
	float LaunchStrength = 1000.f;

	UPROPERTY(EditAnywhere, Category = "FishingGame|Hook")
	float PitchAngle = 35.f;;

	UPROPERTY(EditDefaultsOnly, Category = "FishingGame|Settings")
	float FishingWaitTime = 3.f;

	bool bFishBiting = false;

	FTimerHandle FishBiteTimerHandle;
	FTimerHandle FailTimerHandle;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FishingGame|Model", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FishingRod;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FishingGame|Model", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Hook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FishingGame|Model", meta = (AllowPrivateAccess = "true"))
	class UCableComponent* RodLine;
};

