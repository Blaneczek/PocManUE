// Fill out your copyright notice in the Description page of Project Settings.

#include "PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PMPlayerController.h"
#include "PMPlayer.h"
#include "PMGhost.h"

void APMGameModeBase::AddPoints_Implementation(int32 points)
{
	Score += points;

	if (NumberOfCoins == 0)
	{
		StopGame();
		WonGame();
	}
}

void APMGameModeBase::HandleGhostHit_Implementation()
{
	StopGame();

	Lives--;
	if (Lives == 0)
	{
		LostGame();
		return;
	}

	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMGameModeBase::StartGame, 3.f, false);
}

void APMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("game mode begin play"));

	Player = Cast<APMPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::BeginPlay | Player is nullptr"));
	}

	TArray<AActor*> ghosts;
	UGameplayStatics::GetAllActorsOfClass(this, GhostClass, ghosts);
	for (AActor* item : ghosts)
	{
		APMGhost* ghost = Cast<APMGhost>(item);
		if (ghost != nullptr)
		{
			Ghosts.Add(ghost);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::BeginPlay | Ghost is nullptr"));
		}
	}

	APMPlayerController* PC = Cast<APMPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC != nullptr)
	{
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void APMGameModeBase::StartGame()
{	
	if (Player != nullptr)
	{
		Player->StartPlayer();
	}

	for (APMGhost* ghost : Ghosts)
	{
		if (ghost != nullptr)
		{
			ghost->StartGhost();
		}
	}
}

void APMGameModeBase::StopGame()
{
	if (Player != nullptr)
	{
		Player->ResetPlayer();
	}

	for (APMGhost* ghost : Ghosts)
	{
		if (ghost != nullptr)
		{
			ghost->ResetGhost();
		}
	}
}

void APMGameModeBase::StopAllMovement()
{
	if (Player != nullptr)
	{
		Player->StopMovement();
	}

	for (APMGhost* ghost : Ghosts)
	{
		if (ghost != nullptr)
		{
			ghost->StopMovement();
		}
	}

	FTimerHandle StartMovementTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartMovementTimerHandle, this, &APMGameModeBase::StartAllMovement, 0.5, false);
}

void APMGameModeBase::StartAllMovement()
{
	if (Player != nullptr)
	{
		Player->StartMovement();
	}

	for (APMGhost* ghost : Ghosts)
	{
		if (ghost != nullptr)
		{
			ghost->StartMovement();
		}
	}
}

void APMGameModeBase::RestartGame()
{
	APMPlayerController* PC = Cast<APMPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC != nullptr)
	{
		PC->RestartLevel();
		//TODO: open level menu
	}	
}

void APMGameModeBase::AddCoin()
{
	NumberOfCoins++;
}

void APMGameModeBase::SubtractCoin()
{
	NumberOfCoins--;
}

void APMGameModeBase::PlayerAttackState()
{
	for (APMGhost* ghost : Ghosts)
	{
		if (ghost != nullptr)
		{
			ghost->VulnerableState();
		}
	}
}
