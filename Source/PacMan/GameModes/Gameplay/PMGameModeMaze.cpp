// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGameModeMaze.h"
#include "UI/HUD/PMMazeHUD.h"
#include "UI/HUD/PMNextLevelWidget.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "Gameplay/Coins/PMMapCoin.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Ghosts/PMGhost.h"
#include "Components/AudioComponent.h"
#include "GameInstance/PMGameInstance.h"

APMGameModeMaze::APMGameModeMaze()
{
	MapsNumber = 2;
	bMapOpen = false;
	bPlayerAlreadyChased = false;
	bStillVulnerable = false;
}

void APMGameModeMaze::BeginPlay()
{
	Super::BeginPlay();

	MapCoinDel.BindUFunction(this, FName("SpawnSpecialCoin"), MapCoinClass);
	GetWorld()->GetTimerManager().SetTimer(MapCoinTimer, MapCoinDel, 10.f, false);
}

void APMGameModeMaze::InitializeWidgets(APlayerController* PlayerController)
{
	if (MazeHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMMazeHUD>(PlayerController, MazeHUDClass);		
		MazeHUD = Cast<UPMMazeHUD>(HUDWidget);
		if (MazeHUD != nullptr)
		{
			MazeHUD->AddToViewport();
			MazeHUD->SetMapDisplay(CurrentLevelNum);		
		}	
	}

	Super::InitializeWidgets(PlayerController);
}

void APMGameModeMaze::PlayerChasedHandle(bool IsPlayerChased)
{
	Super::PlayerChasedHandle(IsPlayerChased);

	if (IsPlayerChased)
	{
		ChasingGhosts.Add(true);
	}
	else if (!IsPlayerChased && ChasingGhosts.Num() > 0)
	{
		ChasingGhosts.Pop(true);
	}

	if (ChasingGhosts.Num() == 1)
	{
		MazeHUD->ShowChaseScreen();
	}
	else if (ChasingGhosts.Num() == 0)
	{
		MazeHUD->HideChaseScreen();
	}
}

void APMGameModeMaze::RestartGameType()
{
	UGameplayStatics::OpenLevel(GetWorld(), *GameInstance->MazeLevels.Find(1));
}

void APMGameModeMaze::SetGameplayValues()
{
	Score = GameInstance->MazeGameData.Score;
	Cherries = GameInstance->MazeGameData.Cherries;
	CurrentLevelNum = GameInstance->MazeGameData.LevelNum;
}

void APMGameModeMaze::EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame)
{
	ClearChasedState();

	if (bWonGame && GameInstance->MazeLevels.Contains(CurrentLevelNum + 1))
	{
		if (NextLevelWidget != nullptr)
		{
			NextLevelWidget->AddToViewport();
		}
		GameInstance->MazeGameData = FGameData(CurrentLevelNum + 1, Score, Cherries);
		GameInstance->SaveGame();

		FTimerHandle NextLevelTimer;
		FTimerDelegate NextLevelDel;
		NextLevelDel.BindUFunction(this, "OpenNextLevel", *GameInstance->MazeLevels.Find(CurrentLevelNum + 1));
		GetWorld()->GetTimerManager().SetTimer(NextLevelTimer, NextLevelDel, 2.f, false);
		return;
	}
	else
	{
		GameInstance->MazeGameData = FGameData(1, 0, 0);
	}

	Super::EndGameHandle(EndGameWidget, EndGameSound, bWonGame);	
}

void APMGameModeMaze::HandleGhostHit()
{
	Super::HandleGhostHit();

	ClearChasedState();
}

void APMGameModeMaze::StartPlayerAttackState()
{
	Super::StartPlayerAttackState();

	if (VulnerableGhostTimer.IsValid())
	{
		if (IsValid(PlayerAttackAC))
		{
			PlayerAttackAC->Stop();
			PlayerAttackAC = UGameplayStatics::SpawnSound2D(GetWorld(), VulnerableSound);
		}
		GetWorld()->GetTimerManager().ClearTimer(VulnerableGhostTimer);
		GetWorld()->GetTimerManager().SetTimer(VulnerableGhostTimer, this, &APMGameModeMaze::EndPlayerAttackState, 7.f, false);
		return;
	}

	ClearChasedState();
	PlayerAttackAC = UGameplayStatics::SpawnSound2D(GetWorld(), VulnerableSound);
	MazeHUD->ShowVulnerableScreen();	
	GetWorld()->GetTimerManager().SetTimer(VulnerableGhostTimer, this, &APMGameModeMaze::EndPlayerAttackState, 7.f, false);
}

void APMGameModeMaze::EndPlayerAttackState()
{
	if (VulnerableGhostTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableGhostTimer);
	}
	if (IsValid(PlayerAttackAC))
	{
		PlayerAttackAC->Stop();
	}

	MazeHUD->HideVulnerableScreen();
}

void APMGameModeMaze::HideMap()
{
	MazeHUD->SetMapVisibility(ESlateVisibility::Hidden);
	bMapOpen = false;
}

void APMGameModeMaze::ShowMap()
{
	if (bMapOpen || MapsNumber == 0)
	{
		return;
	}

	MazeHUD->SetMapVisibility(ESlateVisibility::Visible);
	bMapOpen = true;
	MazeHUD->UpdateMapIcon(MapsNumber, ESlateVisibility::Hidden);
	MapsNumber--;

	GetWorld()->GetTimerManager().SetTimer(MapTimer, this, &APMGameModeMaze::HideMap, 5.f, false);
}

void APMGameModeMaze::AddMap()
{
	if (MapsNumber < 2)
	{
		MapsNumber++;
		MazeHUD->UpdateMapIcon(MapsNumber, ESlateVisibility::Visible);
	}
	else
	{
		MazeHUD->ShowFullMapsText();
	}

	GetWorld()->GetTimerManager().SetTimer(MapCoinTimer, MapCoinDel, 10.f, false);
}

void APMGameModeMaze::ClearChasedState()
{
	ChasingGhosts.Empty();
	MazeHUD->HideChaseScreen();
}

