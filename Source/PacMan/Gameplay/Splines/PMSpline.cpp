// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Coins/PMCoin.h"


// Sets default values
APMSpline::APMSpline()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	SetRootComponent(SceneComponent);
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(SceneComponent);

	Splines.Add(FSplines());
	Splines.Add(FSplines());

}

void APMSpline::BeginPlay()
{
	Super::BeginPlay();

	SpawnCoins();
}

void APMSpline::SpawnCoins()
{
	APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (gameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMPSpline::SpawnCoins | gameMode is nullptr"));
		return;
	}

	if (this->ActorHasTag(FName(TEXT("withoutCoins"))))
	{
		return;
	}

	int32 coinsNumber = FMath::RoundToInt(SplineComponent->GetSplineLength() / Distance);

	for (int32 i = 0; i <= coinsNumber; ++i)
	{		
		const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance * i, ESplineCoordinateSpace::World);
		const FRotator Rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		if (GetWorld()->SpawnActor<APMCoin>(CoinClass, Location, Rotation, SpawnInfo) != nullptr)
		{
			gameMode->AddCoin();
		}

	}
}



