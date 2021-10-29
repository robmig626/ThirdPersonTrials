// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "ThirdPersonTrialsCharacter.h"
#include "ThirdPersonTrialsGameMode.generated.h"

UCLASS(minimalapi)
class AThirdPersonTrialsGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;

	AThirdPersonTrialsGameMode();
	AThirdPersonTrialsCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere)
	APlayerStart* PlayerStartActor;
};



