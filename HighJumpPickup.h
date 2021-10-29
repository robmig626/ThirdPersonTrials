// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HighJumpPickup.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONTRIALS_API AHighJumpPickup : public APickup
{
	GENERATED_BODY()
protected:
	virtual void InteractWithPlayer() override;
};
