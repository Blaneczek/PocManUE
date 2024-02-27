// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMClassicPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameModes/Gameplay/PMGameModeBase.h"

APMClassicPlayer::APMClassicPlayer()
{
	TopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top"));
	TopMesh->SetupAttachment(Mesh);
	BottomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bottom"));
	BottomMesh->SetupAttachment(Mesh);

	AnimationSpeed = 0.2f;
	AnimationDirection = 1.f;
}

void APMClassicPlayer::BeginPlay()
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

void APMClassicPlayer::Tick(float DeltaTime)
{
	if (bIsMoving)
	{
		AnimateMesh(DeltaTime);
	}

	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APMClassicPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Started, this, &APMClassicPlayer::MoveUp);
		EnhancedInputComponent->BindAction(MoveDownAction, ETriggerEvent::Started, this, &APMClassicPlayer::MoveDown);
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &APMClassicPlayer::MoveLeft);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Started, this, &APMClassicPlayer::MoveRight);
	}
}

void APMClassicPlayer::MoveUp()
{
	if (CurrentDirection == EDirection::UPWARD) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::UPWARD;

	if (CurrentDirection == EDirection::DOWN)
	{
		Rotate180(EDirection::UPWARD);
	}
}

void APMClassicPlayer::MoveDown()
{
	if (CurrentDirection == EDirection::DOWN) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::DOWN;

	if (CurrentDirection == EDirection::UPWARD)
	{
		Rotate180(EDirection::DOWN);
	}
}

void APMClassicPlayer::MoveLeft()
{
	if (CurrentDirection == EDirection::LEFT) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::LEFT;

	if (CurrentDirection == EDirection::RIGHT)
	{
		Rotate180(EDirection::LEFT);
	}
}

void APMClassicPlayer::MoveRight()
{
	if (CurrentDirection == EDirection::RIGHT) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::RIGHT;

	if (CurrentDirection == EDirection::LEFT)
	{
		Rotate180(EDirection::RIGHT);
	}
}

void APMClassicPlayer::AnimateMesh(float DeltaTime)
{
	if (AnimationTimeElapsed < AnimationSpeed)
	{
		if (AnimationDirection > 0)
		{
			TopMesh->SetRelativeRotation(FRotator(FMath::Lerp(0.f, -60.f, AnimationTimeElapsed / AnimationSpeed), 0.f, 0.f));
			BottomMesh->SetRelativeRotation(FRotator(FMath::Lerp(0.f, 60.f, AnimationTimeElapsed / AnimationSpeed), 0.f, 0.f));
		}
		else
		{
			TopMesh->SetRelativeRotation(FRotator(FMath::Lerp(-60.f, 0.f, AnimationTimeElapsed / AnimationSpeed), 0.f, 0.f));
			BottomMesh->SetRelativeRotation(FRotator(FMath::Lerp(60.f, 0.f, AnimationTimeElapsed / AnimationSpeed), 0.f, 0.f));
		}
		
		AnimationTimeElapsed += DeltaTime;
	}
	else
	{
		AnimationDirection *= -1.f;
		AnimationTimeElapsed = 0;
	}
}
