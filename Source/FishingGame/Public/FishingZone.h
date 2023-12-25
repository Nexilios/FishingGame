// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishingZone.generated.h"

UCLASS()
class FISHINGGAME_API AFishingZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AFishingZone();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UBoxComponent* BoxComp;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
