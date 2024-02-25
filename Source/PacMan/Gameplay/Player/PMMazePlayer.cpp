// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazePlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
		case EDirection::UPWARD:
		{
			DesiredDirection = EDirection::LEFT;
			break;
		}
		case EDirection::DOWN:
		{
			DesiredDirection = EDirection::RIGHT;
			break;
		}
		case EDirection::LEFT:
		{
			DesiredDirection = EDirection::DOWN;
			break;
		}
		case EDirection::RIGHT:
		{			
			DesiredDirection = EDirection::UPWARD;
			break;
		}
		default: break;
	}

	TempDirection = EDirection::NONE;
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
		case EDirection::UPWARD:
		{
			DesiredDirection = EDirection::RIGHT;
			break;
		}
		case EDirection::DOWN:
		{
			DesiredDirection = EDirection::LEFT;
			break;
		}
		case EDirection::LEFT:
		{
			DesiredDirection = EDirection::UPWARD;
			break;
		}
		case EDirection::RIGHT:
		{
			DesiredDirection = EDirection::DOWN;
			break;
		}
		default: break;
	}

	TempDirection = EDirection::NONE;
}

void APMMazePlayer::TurnAround()
{
	switch (CurrentDirection)
	{
		case EDirection::UPWARD:
		{			
			Rotate(90.f, EDirection::DOWN);
			DesiredDirection = EDirection::DOWN;
			break;
		}
		case EDirection::DOWN:
		{			
			Rotate(-90.f, EDirection::UPWARD);
			DesiredDirection = EDirection::UPWARD;
			break;
		}
		case EDirection::LEFT:
		{			
			Rotate(0.f, EDirection::RIGHT);
			DesiredDirection = EDirection::RIGHT;
			break;
		}
		case EDirection::RIGHT:
		{			
			Rotate(180.f, EDirection::LEFT);
			DesiredDirection = EDirection::LEFT;			
			break;
		}
		default: return;
	}

	TempDirection = EDirection::NONE;
}

void APMMazePlayer::OpenMap()
{
	if (APMGameModeMaze* GM = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->ShowMap();
	}
}
