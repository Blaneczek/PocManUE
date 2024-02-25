// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "Gameplay/Ghosts/PMClassicGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "GameModes/Gameplay/PMGameModeBase.h"

APMClassicGhost::APMClassicGhost()
{
	DummyScene = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy scenne"));
	DummyScene->SetupAttachment(RootComponent);
	DummyScene->SetUsingAbsoluteRotation(true);

	Mesh->SetupAttachment(DummyScene);

	EyeR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eye_R"));
	EyeR->SetupAttachment(Mesh);
	EyeL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eye_L"));
	EyeL->SetupAttachment(Mesh);
	IrisR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Iris_R"));
	IrisR->SetupAttachment(EyeR);
	IrisL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Iris_L"));
	IrisL->SetupAttachment(EyeL);
}

void APMClassicGhost::BeginPlay()
{
	Super::BeginPlay();

	switch (GameMode->CurrentLevelNum)
	{
		case 2:
		{
			Speed += 50;
			break;
		}
		case 3:
		{
			Speed += 70;
			break;
		}
		default: break;
	}
}

void APMClassicGhost::SetEyesPosition(const int32 YawRotation)
{
	switch (YawRotation)
	{
		case 0: 
		{
			SetEyes(FVector(0, 0, 0), FVector(0, 0, 0), FVector(-19, 0, -5), FVector(-13, 0, -5));
			return;
		}
		case 90:
		{
			SetEyes(FVector(0, 0, 0), FVector(0, 0, 0), FVector(-11, 0, -16), FVector(-5, 0, -16));
			return;
		}
		case -90:
		{
			SetEyes(FVector(0, 0, 26), FVector(0, 0, 28), FVector(-11, 0, 8), FVector(-5, 0, 8));
			return;
		}
		case 180:
		{
			SetEyes(FVector(0, 0, 0), FVector(0, 0, 0), FVector(-5, 0, -5), FVector(0, 0, -5));
			return;
		}

	}
}

void APMClassicGhost::SetEyes(const FVector& InEyeL, const FVector& InEyeR, const FVector& InIrisL, const FVector& InIrisR)
{
	EyeL->SetRelativeLocation(InEyeL);
	EyeR->SetRelativeLocation(InEyeR);
	IrisL->SetRelativeLocation(InIrisL);
	IrisR->SetRelativeLocation(InIrisR);
}
