// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "GameInstance/PMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Coins/PMCoin.h"


// Sets default values
APMSpline::APMSpline()
{
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

	if (this->ActorHasTag(FName(TEXT("withoutCoins"))))
	{
		return;
	}

	const int32 CoinsNumber = FMath::RoundToInt(SplineComponent->GetSplineLength() / CoinDistanceOnSpline);

	for (int32 i = 0; i <= CoinsNumber; ++i)
	{		
		const FVector& Location = SplineComponent->GetLocationAtDistanceAlongSpline(CoinDistanceOnSpline * i, ESplineCoordinateSpace::World);
		const FRotator& Rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

		switch (UPMGameInstance::GetCurrentLevelType())
		{
			case ELevelType::CLASSIC:
			{
				GetWorld()->SpawnActor<APMCoin>(ClassicCoinClass, Location, Rotation, SpawnInfo);
				break;
			}
			case ELevelType::MAZE:
			{
				GetWorld()->SpawnActor<APMCoin>(MazeCoinClass, Location, Rotation, SpawnInfo);
				break;
			}
			default: break;
		}
			
				
	}
}



