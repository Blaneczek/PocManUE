// Fill out your copyright notice in the Description page of Project Settings.

#include "PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PMPlayer.h"
#include "PMGhost.h"

void APMGameModeBase::AddPoints_Implementation(int32 points)
{
	Score += points;
	//update widget
}

void APMGameModeBase::HandleGhostHit_Implementation()
{
	HealthPoints--;
	if (HealthPoints == 0)
	{
		EndGame();
		return;
	}

	APMPlayer* player = Cast<APMPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::HandleGhostHit | Player is nullptr"));
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
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::HandleGhostHit | Ghost is nullptr"));
		}		
	}
	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMGameModeBase::StartGame, 4.f, false);
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
	UE_LOG(LogTemp, Warning, TEXT("cekekeke"));
}

void APMGameModeBase::EndGame()
{

}
