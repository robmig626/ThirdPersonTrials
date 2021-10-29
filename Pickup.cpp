// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ThirdPersonTrialsCharacter.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundBase.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionBox->SetBoxExtent(FVector(32.f, 32.f, 32.f));
	CollisionBox->SetCollisionProfileName("Trigger");
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlapBegin);

	AudioOnPickUp = CreateDefaultSubobject<UAudioComponent>(TEXT("Pick Up Sound"));
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	TrialsCharacter = Cast<AThirdPersonTrialsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	GameMode = UGameplayStatics::GetGameMode(GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("Pickup is Here"));
	AudioOnPickUp->SetAutoActivate(false);
	AudioOnPickUp->SetSound(SoundOnPickUp);
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(QuatRotation,false,nullptr,ETeleportType::None);
}


void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult &SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Achieved!"));
	if (TrialsCharacter && TrialsCharacter == OtherActor)
	{
		InteractWithPlayer();
		//UE_LOG(LogTemp, Warning, TEXT("Character Overlapping!"));
		//TrialsCharacter->SpeedUp();
		//TrialsCharacter->SpeedUpPickupTimerFunction();
	}
}

void APickup::InteractWithPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("Pickup Interacting with Player!!!"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TimerFunction();
	if (SoundOnPickUp && AudioOnPickUp)
	{
		AudioOnPickUp->Play(0.f);
	}
}

void APickup::Respawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Respawned!"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->SetActorHiddenInGame(false);
}

void APickup::TimerFunction()
{
	if (bCanRespawn)
	{
		GetWorldTimerManager().SetTimer(PickUpTimerHandle, this, &APickup::Respawn, RespawnTime, false, RespawnTime);
	}
}