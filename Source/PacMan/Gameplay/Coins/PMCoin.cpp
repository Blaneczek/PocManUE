// Fill out your copyright notice in the Description page of Project Settings.


#include "PMCoin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Player/PMPlayer.h"

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

}

// Called every frame
void APMCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 APMCoin::Interaction()
{
	UE_LOG(LogTemp, Warning, TEXT("overlap coin"));
	APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (gameMode != nullptr)
	{
		gameMode->SubtractCoin();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMCoin::Interaction | gameMode is nullptr"))
	}
	Destroy();
	return 10;
}


