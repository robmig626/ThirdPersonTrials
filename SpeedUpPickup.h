// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedUpPickup.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONTRIALS_API ASpeedUpPickup : public APickup
{
	GENERATED_BODY()
public:
	virtual void InteractWithPlayer() override;

};
