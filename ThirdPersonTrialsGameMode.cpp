// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonTrialsGameMode.h"
#include "ThirdPersonTrialsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AThirdPersonTrialsGameMode::AThirdPersonTrialsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PrimaryActorTick.bCanEverTick = true;
}

void AThirdPersonTrialsGameMode::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<AThirdPersonTrialsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AThirdPersonTrialsGameMode::Tick(float DeltaTime)
{
	if (!UGameplayStatics::GetPlayerCharacter(AActor::GetWorld(),0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Reloading Level"));
		UGameplayStatics::OpenLevel(GetWorld(), (FName)UGameplayStatics::GetCurrentLevelName(GetWorld()), true);
	}
}