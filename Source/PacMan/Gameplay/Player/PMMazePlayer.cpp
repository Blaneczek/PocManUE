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
	switch (CurrentDirection)
	{
		case EDirection::UPWARD:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::DOWN;
			RotatePlayer(90.f, EDirection::DOWN);
			return;
		}
		case EDirection::DOWN:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::UPWARD;
			RotatePlayer(-90.f, EDirection::UPWARD);
			return;
		}
		case EDirection::LEFT:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::RIGHT;
			RotatePlayer(0.f, EDirection::RIGHT);
			return;
		}
		case EDirection::RIGHT:
		{
			TempDirection = EDirection::NONE;
			DesiredDirection = EDirection::LEFT;
			RotatePlayer(180.f, EDirection::LEFT);
			return;
		}
		default: return;
	}
}

void APMMazePlayer::OpenMap()
{
	if (APMGameModeMaze* GM = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(this)))
	{
		GM->ShowMap();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMMazePlayer::OpenMap | GameMode is nullptr"));
	}
}
