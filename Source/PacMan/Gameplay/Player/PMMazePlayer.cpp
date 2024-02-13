// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazePlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APMMazePlayer::APMMazePlayer()
{
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->SetupAttachment(Mesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm);
}

// Called to bind functionality to input
void APMMazePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &APMMazePlayer::MoveLeft);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Started, this, &APMMazePlayer::MoveRight);
		EnhancedInputComponent->BindAction(TurnAroundAction, ETriggerEvent::Started, this, &APMMazePlayer::TurnAround);
	}
}

void APMMazePlayer::MoveLeft()
{
	/**
	* Counterclockwise (e.g. if CurrentDirection is RIGHT, DesiredDirection will be UPWARD)
	* 
	*				UPWARD
	*		    LEFT	  RIGHT
	*				 DOWN
	*/
	switch (CurrentDirection)
	{
		case EDirection::UPWARD:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::LEFT;
			return;
		}
		case EDirection::DOWN:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::RIGHT;
			return;
		}
		case EDirection::LEFT:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::DOWN;
			return;
		}
		case EDirection::RIGHT:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::UPWARD;
			return;
		}
		default: return;
	}
}

void APMMazePlayer::MoveRight()
{
	/**
	* Clockwise (e.g. if CurrentDirection is RIGHT, DesiredDirection will be DOWN)
	*
	*				UPWARD
	*		    LEFT	  RIGHT
	*				 DOWN
	*/
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(CurrentDirection));
	switch (CurrentDirection)
	{
		case EDirection::UPWARD:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::RIGHT;
			return;
		}
		case EDirection::DOWN:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::LEFT;
			return;
		}
		case EDirection::LEFT:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::UPWARD;
			return;
		}
		case EDirection::RIGHT:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::DOWN;
			return;
		}
		default: return;
	}
}

void APMMazePlayer::TurnAround()
{
	if (CurrentDirection == EDirection::LEFT) return;

	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::LEFT;

	if (CurrentDirection == EDirection::RIGHT)
	{
		RotatePlayer(180.f, EDirection::LEFT);
	}
}
