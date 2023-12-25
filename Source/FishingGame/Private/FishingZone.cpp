// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingZone.h"
#include "Components/BoxComponent.h"
#include "FishingGameCharacter.h"
#include "FishingGamePlayerController.h"

AFishingZone::AFishingZone()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Zone"));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
}

void AFishingZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (AFishingGameCharacter* PCharacter = Cast<AFishingGameCharacter>(OtherActor))
	{
		if (AFishingGamePlayerController* PController = Cast<AFishingGamePlayerController>(PCharacter->GetController()))
		{
			if (!PController->GetIsFishing() && PController->GetInTransition())
			{
				if (UStaticMeshComponent* Hook = PCharacter->GetHookMesh())
				{
					Hook->SetSimulatePhysics(false);
					Hook->SetMobility(EComponentMobility::Static);
				}
			}
		}
	}
}

