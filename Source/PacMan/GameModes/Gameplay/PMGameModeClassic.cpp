// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGameModeClassic.h"
#include "UI/HUD/PMClassicHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Gameplay/Ghosts/PMGhost.h"
#include "GameInstance/PMGameInstance.h"

APMGameModeClassic::APMGameModeClassic()
{

}

void APMGameModeClassic::BeginPlay()
{
	Super::BeginPlay();

	GhostAudio = UGameplayStatics::CreateSound2D(this, GhostSound, 1.f, 1.f, 0.f, nullptr, false, false);
}

void APMGameModeClassic::InitStartingWidgets()
{
	if (ClassicHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMClassicHUD>(GetWorld(), ClassicHUDClass);
		HUDWidget->SetScore(Score);
		HUDWidget->SetCherries(Cherries);
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeClassic::InitStartingWidgets | ClassicHUDClass is nullptr"));
	}

	Super::InitStartingWidgets();
}

void APMGameModeClassic::HandleEndGame(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, USoundWave* EndGameSound, bool bWonGame)
{
	if (bWonGame && GameInstance->ClassicLevels.Contains(CurrentLevelNum + 1)) // Next level
	{
		CreateNextLevelWidget();

		GameInstance->ClassicGameData = FGameData(CurrentLevelNum + 1, Score, Cherries);
		GameInstance->SaveGame();

		const FName NextLevelName = *GameInstance->ClassicLevels.Find(CurrentLevelNum + 1);
		FTimerHandle NextLevelTimer;
		FTimerDelegate NextLevelDel;
		NextLevelDel.BindUFunction(this, FName(TEXT("OpenNextLevel")), NextLevelName);
		GetWorld()->GetTimerManager().SetTimer(NextLevelTimer, NextLevelDel, 2.f, false);
		return;
	}
	else
	{
		GameInstance->ClassicGameData = FGameData(1, 0, 0);
	}

	Super::HandleEndGame(EndGameWidgetClass, EndGameSound, bWonGame);
}

void APMGameModeClassic::SetGameplayValues()
{
	Score = GameInstance->ClassicGameData.Score;
	Cherries = GameInstance->ClassicGameData.Cherries;
	CurrentLevelNum = GameInstance->ClassicGameData.LevelNum;
}


void APMGameModeClassic::StartAllMovement()
{
	Super::StartAllMovement();

	if (!IsValid(GhostAudio)) return;

	if (!GhostAudio->IsPlaying())
	{
		GhostAudio->Play();
	}
}

void APMGameModeClassic::StopGame()
{
	Super::StopGame();

	if (!IsValid(GhostAudio)) return;

	if (GhostAudio->IsPlaying())
	{
		GhostAudio->Stop();
	}
}

void APMGameModeClassic::RestartGameType()
{
	UGameplayStatics::OpenLevel(this, *GameInstance->ClassicLevels.Find(1));
}

void APMGameModeClassic::StartPlayerAttackState()
{
	Super::StartPlayerAttackState();

	if (VulnerableGhostTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableGhostTimer);
		GetWorld()->GetTimerManager().SetTimer(VulnerableGhostTimer, this, &APMGameModeClassic::EndPlayerAttackState, 7.f, false);
		return;
	}
	
	if (IsValid(GhostAudio))
	{
		GhostAudio->SetSound(VulnerableGhostSound);
	}

	GetWorld()->GetTimerManager().SetTimer(VulnerableGhostTimer, this, &APMGameModeClassic::EndPlayerAttackState, 7.f, false);
}

void APMGameModeClassic::EndPlayerAttackState()
{
	Super::EndPlayerAttackState();

	if (IsValid(GhostAudio))
	{
		GhostAudio->SetSound(GhostSound);
	}

	if (VulnerableGhostTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableGhostTimer);
	}
}


