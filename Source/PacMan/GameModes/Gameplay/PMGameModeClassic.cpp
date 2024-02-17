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


	UE_LOG(LogTemp, Warning, TEXT("classic game mode"));
	GhostAudio = UGameplayStatics::CreateSound2D(this, GhostSound, 1.f, 1.f, 0.f, nullptr, false, false);

}

void APMGameModeClassic::InitializeWidgets(APlayerController* PlayerController)
{
	Super::InitializeWidgets(PlayerController);

	if (ClassicHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMClassicHUD>(PlayerController, ClassicHUDClass);
		HUDWidget->SetValues(Score, CherryNumber);
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}
}

void APMGameModeClassic::EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame)
{
	if (bWonGame && GameInstance->ClassicLevels.Contains(CurrentLevelNum + 1))
	{
		//add to viewport NEXT LEVEL widget;
		GameInstance->ClassicGameData = FGameData(CurrentLevelNum + 1, Score, CherryNumber);
		GameInstance->SaveGame();
		UGameplayStatics::OpenLevel(this, *GameInstance->ClassicLevels.Find(CurrentLevelNum + 1));
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
	CherryNumber = GameInstance->ClassicGameData.CherryNumber;
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

void APMGameModeClassic::PlayerAttackState()
{
	Super::PlayerAttackState();

	if (!IsValid(GhostAudio)) return;

	GhostAudio->SetSound(VulnerableGhostSound);
}

void APMGameModeClassic::EndPlayerAttackState()
{
	Super::EndPlayerAttackState();

	for (auto ghost : Ghosts)
	{
		if (ghost->IsVulnerable()) return;
	}

	if (!IsValid(GhostAudio)) return;

	GhostAudio->SetSound(GhostSound);
}


