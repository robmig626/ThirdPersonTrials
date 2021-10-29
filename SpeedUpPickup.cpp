// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedUpPickup.h"
#include "ThirdPersonTrialsCharacter.h"
#include "TimerManager.h"

void ASpeedUpPickup::InteractWithPlayer()
{
	Super::InteractWithPlayer();
	TrialsCharacter->bSpeedUpPickedUp = true;
	TrialsCharacter->SpeedUp();
	TrialsCharacter->SpeedUpPickupTimerFunction();
	this->SetActorHiddenInGame(true);
}
