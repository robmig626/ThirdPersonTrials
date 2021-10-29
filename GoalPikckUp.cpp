// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalPikckUp.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void AGoalPikckUp::InteractWithPlayer()
{
	GEngine->AddOnScreenDebugMessage(6, 5, FColor::Magenta, TEXT("You Win!!!"));
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}