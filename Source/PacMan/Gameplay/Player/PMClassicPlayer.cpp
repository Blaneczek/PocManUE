// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "Gameplay/Player/PMClassicPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
		RotatePlayer(-90.f, EDirection::UPWARD);
	}
}

void APMClassicPlayer::MoveDown()
{
	if (CurrentDirection == EDirection::DOWN) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::DOWN;

	if (CurrentDirection == EDirection::UPWARD)
	{
		RotatePlayer(90.f, EDirection::DOWN);
	}
}

void APMClassicPlayer::MoveLeft()
{
	if (CurrentDirection == EDirection::LEFT) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::LEFT;

	if (CurrentDirection == EDirection::RIGHT)
	{
		RotatePlayer(180.f, EDirection::LEFT);
	}
}

void APMClassicPlayer::MoveRight()
{
	if (CurrentDirection == EDirection::RIGHT) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::RIGHT;

	if (CurrentDirection == EDirection::LEFT)
	{
		RotatePlayer(0.f, EDirection::RIGHT);
	}
}