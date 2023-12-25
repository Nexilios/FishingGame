// Copyright Epic Games, Inc. All Rights Reserved.

#include "FishingGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "CableComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "FishingGamePlayerController.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"

AFishingGameCharacter::AFishingGameCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-75.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = true;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FishingRod = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fishing Rod"));
	FishingRod->SetupAttachment(GetMesh(), "LeftHandSocket");
	FishingRod->SetRelativeLocation(FVector(5.235499, -2.556474, 1.357719));
	FishingRod->SetRelativeRotation(FRotator(76.808014, 89.998528, -34.547497));

	RodLine = CreateDefaultSubobject<UCableComponent>(TEXT("Rod Line"));
	RodLine->SetupAttachment(FishingRod, "TipSocket");
	RodLine->CableLength = 30.f;
	RodLine->CableWidth = 2.f;
	RodLine->bEnableStiffness = true;
	RodLine->bAttachEnd = false;
	RodLine->EndLocation = FVector::ZeroVector;

	Hook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook"));
	Hook->SetupAttachment(RodLine, "CableEnd");
	Hook->SetRelativeScale3D(FVector(0.025f, 0.025f, 0.025f));
	Hook->SetCollisionResponseToAllChannels(ECR_Block);
	Hook->SetCollisionObjectType(ECC_GameTraceChannel1); // Set Hook object type to "Hook"

	FishBiteFXComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bite VFX"));
	FishBiteFXComp->SetupAttachment(Hook);
	FishBiteFXComp->SetAutoActivate(false);
	FishBiteFXComp->SetRelativeScale3D(FVector(5.f, 5.f, 6.f));
	FishBiteFXComp->SetUsingAbsoluteRotation(true); // Make sure it always face up

	FishMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Fish Mesh"));
	FishMesh->SetupAttachment(Hook, "FishSocket");
	FishMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FishMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	FishMesh->SetVisibility(false);
	FishMesh->SetCastShadow(false);
	FishMesh->SetUsingAbsoluteScale(true);
	FishMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/FishingGame/Assets/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AFishingGameCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (APlayerController* const PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}

void AFishingGameCharacter::LaunchHook()
{
	if (AFishingGamePlayerController* const PC = Cast<AFishingGamePlayerController>(GetController()))
	{
		PC->RemoveCastingWidget();

		FRotator LaunchDirection = GetActorRotation();
		LaunchDirection.Pitch += PitchAngle;
		// Set Launch Velocity with a minimum strength of 300.f
		FVector LaunchVelocity = LaunchDirection.Vector() * FMath::Max(300.f, LaunchStrength * PC->GetCastingProgress());

		Hook->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		Hook->SetSimulatePhysics(true);
		RodLine->bAttachEnd = true;
		RodLine->SetAttachEndToComponent(Hook, "HookSocket");
		Hook->SetPhysicsLinearVelocity(LaunchVelocity, false);
	}
}

void AFishingGameCharacter::ReelHook()
{
	RodLine->bAttachEnd = false;

	if (bFishBiting)
	{
		FishMesh->SetVisibility(true);
	}
	else
	{
		FishMesh->SetVisibility(false);
	}

	Hook->SetSimulatePhysics(false);
	Hook->AttachToComponent(RodLine, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "CableEnd");;
	Hook->SetRelativeLocation(FVector::ZeroVector);
	Hook->SetRelativeRotation(FRotator::ZeroRotator);
}

void AFishingGameCharacter::ClearTimersAndVFX()
{
	Hook->SetMobility(EComponentMobility::Movable); //SetMobility here instead of ReelHook() to avoid racing condition
	GetWorldTimerManager().ClearTimer(FishBiteTimerHandle);
	GetWorldTimerManager().ClearTimer(FailTimerHandle);
	FishBiteFXComp->Deactivate();
}

void AFishingGameCharacter::StartFishing()
{
	if (AFishingGamePlayerController* const PC = Cast<AFishingGamePlayerController>(GetController()))
	{
		PC->SetCastingProgress(0.f);
		if (!bFishBiting && PC->GetIsFishing())
		{
			GetWorldTimerManager().SetTimer(FishBiteTimerHandle, this, &AFishingGameCharacter::FishBite, FishingWaitTime, false);
		}
		else if(bFishBiting && PC->GetIsFishing() && !PC->GetInTransition())
		{
			FishBiteFXComp->Deactivate();
			bFishBiting = false;
			GetWorldTimerManager().SetTimer(FishBiteTimerHandle, this, &AFishingGameCharacter::FishBite, FishingWaitTime, false);
		}
	}
}

void AFishingGameCharacter::FishBite()
{
	if (AFishingGamePlayerController* const PC = Cast<AFishingGamePlayerController>(GetController()))
	{
		if (PC->GetIsFishing())
		{
			bFishBiting = true;
			FishBiteFXComp->Activate(true);
			GetWorldTimerManager().SetTimer(FailTimerHandle, this, &AFishingGameCharacter::StartFishing, 2.f, false); // Wait for 2 second until fish swim away
		}
	}
}
