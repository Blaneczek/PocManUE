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
}

void APMGameModeBase::StartGame()
{	
	APMPlayer* player = Cast<APMPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::StartGame | Player is nullptr"));
		return;
	}

	player->StartPlayer();

	TArray<AActor*> ghosts;
	UGameplayStatics::GetAllActorsOfClass(this, GhostClass, ghosts);
	for (AActor* item : ghosts)
	{
		APMGhost* ghost = Cast<APMGhost>(item);
		if (ghost != nullptr)
		{
			ghost->StartGhost();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::StartGame | Ghost is nullptr"));
		}
	}
}

void APMGameModeBase::StopGame()
{
	APMPlayer* player = Cast<APMPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::StopGame| Player is nullptr"));
		return;
	}
	player->ResetPlayer();

	TArray<AActor*> ghosts;
	UGameplayStatics::GetAllActorsOfClass(this, GhostClass, ghosts);
	for (AActor* item : ghosts)
	{
		APMGhost* ghost = Cast<APMGhost>(item);
		if (ghost != nullptr)
		{
			ghost->ResetGhost();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::StopGame | Ghost is nullptr"));
		}
	}
}

void APMGameModeBase::RestartGame()
{
	APMPlayerController* PC = Cast<APMPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC != nullptr)
	{
		PC->RestartLevel();
		//open level: menu
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
