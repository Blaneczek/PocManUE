// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazePlayer.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "Kismet/GameplayStatics.h"

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
		EnhancedInputComponent->BindAction(OpenMapAction, ETriggerEvent::Started, this, &APMMazePlayer::OpenMap);
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
		case EPlayerDirection::UPWARD:
		{
			DesiredDirection = EPlayerDirection::LEFT;
			break;
		}
		case EPlayerDirection::DOWN:
		{
			DesiredDirection = EPlayerDirection::RIGHT;
			break;
		}
		case EPlayerDirection::LEFT:
		{
			DesiredDirection = EPlayerDirection::DOWN;
			break;
		}
		case EPlayerDirection::RIGHT:
		{			
			DesiredDirection = EPlayerDirection::UPWARD;
			break;
		}
		default: break;
	}

	TempDirection = EPlayerDirection::NONE;
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
	switch (CurrentDirection)
	{
		case EPlayerDirection::UPWARD:
		{
			DesiredDirection = EPlayerDirection::RIGHT;
			break;
		}
		case EPlayerDirection::DOWN:
		{
			DesiredDirection = EPlayerDirection::LEFT;
			break;
		}
		case EPlayerDirection::LEFT:
		{
			DesiredDirection = EPlayerDirection::UPWARD;
			break;
		}
		case EPlayerDirection::RIGHT:
		{
			DesiredDirection = EPlayerDirection::DOWN;
			break;
		}
		default: break;
	}

	TempDirection = EPlayerDirection::NONE;
}

void APMMazePlayer::TurnAround()
{
	switch (CurrentDirection)
	{
		case EPlayerDirection::UPWARD:
		{			
			Rotate180(EPlayerDirection::DOWN);
			DesiredDirection = EPlayerDirection::DOWN;
			break;
		}
		case EPlayerDirection::DOWN:
		{			
			Rotate180(EPlayerDirection::UPWARD);
			DesiredDirection = EPlayerDirection::UPWARD;
			break;
		}
		case EPlayerDirection::LEFT:
		{			
			Rotate180(EPlayerDirection::RIGHT);
			DesiredDirection = EPlayerDirection::RIGHT;
			break;
		}
		case EPlayerDirection::RIGHT:
		{			
			Rotate180(EPlayerDirection::LEFT);
			DesiredDirection = EPlayerDirection::LEFT;			
			break;
		}
		default: return;
	}

	TempDirection = EPlayerDirection::NONE;
}

void APMMazePlayer::OpenMap()
{
	if (APMGameModeMaze* GM = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->ShowMap();
	}
}
