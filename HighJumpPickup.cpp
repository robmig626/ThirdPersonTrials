// Fill out your copyright notice in the Description page of Project Settings.


#include "HighJumpPickup.h"
#include "Engine/Engine.h"
#include "ThirdPersonTrialsCharacter.h"

void AHighJumpPickup::InteractWithPlayer()
{
	Super::InteractWithPlayer();
	TrialsCharacter->bHighJumpSet = true;
	UE_LOG(LogTemp, Warning, TEXT("High Jump Set!"));
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, TEXT("High Jump Set!"));
	this->SetActorHiddenInGame(true);
}