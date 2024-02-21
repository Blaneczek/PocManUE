// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMCoin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "GameInstance/PMGameInstance.h"

// Sets default values
APMCoin::APMCoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
}

// Called when the game starts or when spawned
void APMCoin::BeginPlay()
{
	Super::BeginPlay();

	SetMaterial();
}

void APMCoin::SetMaterial()
{
	if (UPMGameInstance* GI = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		switch (GI->GetCurrentLevelType())
		{
			case ELevelType::CLASSIC:
			{
				Mesh->SetMaterial(0, ClassicMaterial);
				break;
			}
			case ELevelType::MAZE:
			{
				Mesh->SetMaterial(0, MazeMaterial);
				break;
			}
		}
	}
}

// Called every frame
void APMCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 APMCoin::Interaction()
{
	if (APMGameModeBase* GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->SubtractCoin();	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMCoin::Interaction | gameMode is nullptr"))
	}
	Destroy();
	return 10;
}


