// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGameModeClassic.h"
#include "UI/HUD/PMClassicHUD.h"
#include "UI/HUD/PMNextLevelWidget.h"
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

void APMGameModeClassic::InitializeWidgets(APlayerController* PlayerController)
{
	if (ClassicHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMClassicHUD>(PlayerController, ClassicHUDClass);
		HUDWidget->SetScore(Score);
		HUDWidget->SetCherries(Cherries);
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}

	Super::InitializeWidgets(PlayerController);
}

void APMGameModeClassic::EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame)
{
	if (bWonGame && GameInstance->ClassicLevels.Contains(CurrentLevelNum + 1)) // Next level
	{
		if (NextLevelWidget != nullptr)
		{
			NextLevelWidget->AddToViewport();
		}		
		GameInstance->ClassicGameData = FGameData(CurrentLevelNum + 1, Score, Cherries);
		GameInstance->SaveGame();

		FTimerHandle NextLevelTimer;
		FTimerDelegate NextLevelDel;
		NextLevelDel.BindUFunction(this, FName("OpenNextLevel"), *GameInstance->ClassicLevels.Find(CurrentLevelNum + 1));
		GetWorld()->GetTimerManager().SetTimer(NextLevelTimer, NextLevelDel, 2.f, false);
		return;
	}
	else
	{
		GameInstance->ClassicGameData = FGameData(1, 0, 0);
	}

	Super::EndGameHandle(EndGameWidget, EndGameSound, bWonGame);
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


