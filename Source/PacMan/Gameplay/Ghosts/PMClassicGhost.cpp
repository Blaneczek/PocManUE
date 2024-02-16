// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "Gameplay/Ghosts/PMClassicGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

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

void APMClassicGhost::SetEyesPosition(const int32 YawRotation)
{
	switch (YawRotation)
	{
		case 0: 
		{
			SetEyesRight();
			return;
		}
		case 90:
		{
			SetEyesDown();
			return;
		}
		case -90:
		{
			SetEyesUp();
			return;
		}
		case 180:
		{
			SetEyesLeft();
			return;
		}

	}
}

void APMClassicGhost::SetEyesRight()
{
	EyeL->SetRelativeLocation(FVector(0, 0, 0));
	EyeR->SetRelativeLocation(FVector(0, 0, 0));
	IrisL->SetRelativeLocation(FVector(-19, 0, -5));
	IrisR->SetRelativeLocation(FVector(-13, 0, -5));
}

void APMClassicGhost::SetEyesLeft()
{
	EyeL->SetRelativeLocation(FVector(0, 0, 0));
	EyeR->SetRelativeLocation(FVector(0, 0, 0));
	IrisL->SetRelativeLocation(FVector(-5, 0, -5));
	IrisR->SetRelativeLocation(FVector(0, 0, -5));
}

void APMClassicGhost::SetEyesUp()
{
	EyeL->SetRelativeLocation(FVector(0, 0, 26));
	EyeR->SetRelativeLocation(FVector(0, 0, 28));
	IrisL->SetRelativeLocation(FVector(-11, 0, 8));
	IrisR->SetRelativeLocation(FVector(-5, 0, 8));
}

void APMClassicGhost::SetEyesDown()
{
	EyeL->SetRelativeLocation(FVector(0, 0, 0));
	EyeR->SetRelativeLocation(FVector(0, 0, 0));
	IrisL->SetRelativeLocation(FVector(-11, 0, -16));
	IrisR->SetRelativeLocation(FVector(-5, 0, -16));
}
