// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/PMPlayerController.h"
#include "Gameplay/Player/PMPlayer.h"
#include "Gameplay/Ghosts/PMGhost.h"
#include "Gameplay/Coins/PMCoin.h"
#include "Gameplay/Coins/PMCherryCoin.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "UI/HUD/PMHUDWidget.h"
#include "UI/HUD/PMEndGameWidget.h"
#include "UI/HUD/PMStarterWidget.h"
#include "UI/HUD/PMPauseWidget.h"
#include "UI/HUD/PMNextLevelWidget.h"
#include "GameInstance/PMGameInstance.h"
#include "Sound/SoundWave.h"
#include "Saved/PMSaveGame.h"


APMGameModeBase::APMGameModeBase()
{
	Score = 0;
	Lives = 3;
	Cherries = 0;
	bCoinSound = true;
	APMCoin::ResetCoinsNumber();
}

void APMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::BeginPlay | GameInstance is nullptr"));
		return;
	}
	
	CurrentLevelType = GameInstance->GetCurrentLevelType();

	if (GameMusic != nullptr) UGameplayStatics::PlaySound2D(GetWorld(), GameMusic);

	SetGameplayValues();
	SetSplines();
		
	InitializeWidgets(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	FTimerHandle StartGameTimer;
	GetWorld()->GetTimerManager().SetTimer(StartGameTimer, this, &APMGameModeBase::StartGame, 1.f, false);

	GetWorld()->GetTimerManager().SetTimer(StartMovementTimer, this, &APMGameModeBase::StartAllMovement, 3.f, false);

	CherryCoinDel.BindUFunction(this, FName("SpawnSpecialCoin"), CherryCoinClass);
	GetWorld()->GetTimerManager().SetTimer(CherryCoinTimer, CherryCoinDel, 10.f, false);

}

void APMGameModeBase::AddPoints(int32 points)
{
	Score += points;

	if (bCoinSound && (CoinSound != nullptr))
	{
		bCoinSound = false;
		UGameplayStatics::PlaySound2D(GetWorld(), CoinSound);
	}
	else
	{
		bCoinSound = true;
	}

	if (HUDWidget != nullptr)
	{
		HUDWidget->SetScore(Score);
	}

	if (APMCoin::GetCoinsNumber() == 0)
	{
		StopGame();
		EndGameHandle(WinGameWidget, WinGameSound, true);
	}
}

void APMGameModeBase::HandleGhostHit()
{
	StopGame();

	Lives--;
	if (Lives == 0)
	{
		EndGameHandle(LoseGameWidget, LoseGameSound, false);
		return;
	}

	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateLives(Lives);
	}

	if (PlayerHittedSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerHittedSound);
	}

	GetWorld()->GetTimerManager().SetTimer(ResetGameTimer, this, &APMGameModeBase::StartGame, 2.f, false);
	GetWorld()->GetTimerManager().SetTimer(StartMovementTimer, this, &APMGameModeBase::StartAllMovement, 3.f, false);
}

void APMGameModeBase::StartGame()
{	
	OnStartGame.Broadcast();
}

void APMGameModeBase::StopGame()
{
	OnStopGame.Broadcast();
}

void APMGameModeBase::StopAllMovement()
{
	OnStopMovement.Broadcast();

	GetWorld()->GetTimerManager().SetTimer(StartMovementTimerHandle, this, &APMGameModeBase::StartAllMovement, 0.5, false);
}

void APMGameModeBase::StartAllMovement()
{
	OnStartMovement.Broadcast();
}

void APMGameModeBase::OpenPauseMenu()
{
	if (PauseWidget != nullptr)
	{
		PauseWidget->AddToViewport();
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetPause(true);
	}			
}

void APMGameModeBase::ClosePauseMenu()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetPause(false);
	}
	PauseWidget->RemoveFromParent();
}

void APMGameModeBase::EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame)
{
	if (GameInstance != nullptr)
	{
		GameInstance->AddScoreboardData(CurrentLevelType, FScoreboardData(Score, Cherries));
		GameInstance->SaveGame();
	}

	if (EndGameSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EndGameSound);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}

	if (EndGameWidget)
	{
		EndGameWidget->SetFinalScores(Score, Cherries);
		EndGameWidget->AddToViewport();
	}
}

void APMGameModeBase::OpenNextLevel(const FName& LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void APMGameModeBase::GoToMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Menu");
}

void APMGameModeBase::SpawnSpecialCoin(TSubclassOf<APMCoin> SpecialCoinClass)
{
	const int32 randomIndex = FMath::RandRange(0, Splines.Num() - 1);

	if (Splines[randomIndex] != nullptr)
	{
		const float splineLength = Splines[randomIndex]->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1);
		const FVector location = Splines[randomIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(splineLength / 2, ESplineCoordinateSpace::World);
		const FRotator rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APMCoin>(SpecialCoinClass, location, rotation, SpawnInfo);
	}
}

void APMGameModeBase::AddCherryCoin()
{
	GetWorld()->GetTimerManager().SetTimer(CherryCoinTimer, CherryCoinDel, 10.f, false);

	Cherries++;
	if (HUDWidget != nullptr)
	{
		HUDWidget->SetCherries(Cherries);
	}
}

void APMGameModeBase::StartPlayerAttackState()
{
	OnPlayerAttack.Broadcast();
}

void APMGameModeBase::SetPlayerChased(bool IsPlayerChased)
{
	PlayerChasedHandle(IsPlayerChased);
}

void APMGameModeBase::InitializeWidgets(APlayerController* PlayerController)
{
	if (StarterWidgetClass != nullptr)
	{
		StarterWidget = CreateWidget<UPMStarterWidget>(PlayerController, StarterWidgetClass);
		StarterWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | StarterWidgetClass is nullptr"));
	}

	if (LoseGameWidgetClass != nullptr)
	{
		LoseGameWidget = CreateWidget<UPMEndGameWidget>(PlayerController, LoseGameWidgetClass);
		LoseGameWidget->OnBackToMenu.BindUObject(this, &APMGameModeBase::GoToMenu);
		LoseGameWidget->OnRestartGame.BindUObject(this, &APMGameModeBase::RestartGameType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | LoseGameWidgetClass is nullptr"));
	}

	if (WinGameWidgetClass != nullptr)
	{
		WinGameWidget = CreateWidget<UPMEndGameWidget>(PlayerController, WinGameWidgetClass);
		WinGameWidget->OnBackToMenu.BindUObject(this, &APMGameModeBase::GoToMenu);
		WinGameWidget->OnRestartGame.BindUObject(this, &APMGameModeBase::RestartGameType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | WindGameWidgetClass is nullptr"));
	}

	if (PauseWidgetClass != nullptr)
	{
		PauseWidget = CreateWidget<UPMPauseWidget>(PlayerController, PauseWidgetClass);
		PauseWidget->OnBackToMenu.BindUObject(this, &APMGameModeBase::GoToMenu);
		PauseWidget->OnContinue.BindUObject(this, &APMGameModeBase::ClosePauseMenu);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | WindGameWidgetClass is nullptr"));
	}

	if (NextLevelWidgetClass != nullptr)
	{
		NextLevelWidget = CreateWidget<UPMNextLevelWidget>(PlayerController, NextLevelWidgetClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | NextLevelWidgetClass is nullptr"));
	}
}

void APMGameModeBase::SetSplines()
{
	TArray<AActor*> OutSplines;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APMSpline::StaticClass(), OutSplines);
	for (auto& item : OutSplines)
	{
		APMSpline* spline = Cast<APMSpline>(item);
		if (spline && !spline->ActorHasTag(TEXT("withoutCoins")))
		{
			Splines.Add(spline);
		}
	}
}

