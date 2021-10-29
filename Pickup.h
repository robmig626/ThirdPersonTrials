// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonTrialsCharacter.h"
#include "Math/Quat.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Pickup.generated.h"

UCLASS()
class THIRDPERSONTRIALS_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult &SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InteractWithPlayer();
	virtual void TimerFunction();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AThirdPersonTrialsCharacter* TrialsCharacter;
	AGameModeBase* GameMode;
	
	UPROPERTY(EditAnywhere)
	FRotator ObjectRotation = FRotator(0.f, 0.f, 2.f);
	FQuat QuatRotation = FQuat(ObjectRotation);

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
		class USoundCue* SoundOnPickUp;

	UPROPERTY(EditAnywhere)
		class UAudioComponent* AudioOnPickUp;

	UPROPERTY(EditAnywhere)
		bool bCanRespawn = true;

	UPROPERTY(EditAnywhere)
		float RespawnTime = 3.f;

	void Respawn();

	FTimerHandle PickUpTimerHandle;
};
