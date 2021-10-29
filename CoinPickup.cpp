// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinPickup.h"
#include "ThirdPersonTrialsCharacter.h"


void ACoinPickup::InteractWithPlayer()
{
	Super::InteractWithPlayer();
	UE_LOG(LogTemp, Warning, TEXT("You picked up a coin!"));
	TrialsCharacter->PickedUpCoins++;
	TrialsCharacter->ShowCoins();
	this->SetActorHiddenInGame(true);
}