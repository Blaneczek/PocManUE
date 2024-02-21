// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMCoin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

int32 APMCoin::CoinsCounter = 0;

// Sets default values
APMCoin::APMCoin()
{
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
}

// Called when the game starts or when spawned
void APMCoin::BeginPlay()
{
	Super::BeginPlay();

	IncreaseCoinsCounter();
}

void APMCoin::IncreaseCoinsCounter()
{
	CoinsCounter++;
}

int32 APMCoin::Interaction()
{
	CoinsCounter--;
	Destroy();
	return 10;
}


