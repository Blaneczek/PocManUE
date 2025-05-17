// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Coins/PMCoin.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "UI/HUD/PMHUDWidget.h"
#include "UI/HUD/PMEndGameWidget.h"
#include "UI/HUD/PMStarterWidget.h"
#include "UI/HUD/PMPauseWidget.h"
#include "UI/HUD/PMNextLevelWidget.h"
#include "GameInstance/PMGameInstance.h"
#include "Sound/SoundWave.h"


APMGameModeBase::APMGameModeBase()
{
	Score = 0;
	Lives = 3;
	Cherries = 0;
	bCoinSound = true;
	CurrentLevelNum = 0;
	APMCoin::ResetCoinsNumber();
}

void APMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::BeginPlay | GameInstance is nullptr"));
		return;
	}
	
	if (IsValid(GameMusic))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GameMusic);
	}

	SetGameplayValues();
	SetSplines();
		
	InitStartingWidgets();	

	FTimerHandle StartGameTimer;
	GetWorld()->GetTimerManager().SetTimer(StartGameTimer, this, &APMGameModeBase::StartGame, 1.f, false);

	GetWorld()->GetTimerManager().SetTimer(StartMovementTimer, this, &APMGameModeBase::StartAllMovement, 3.f, false);

	CherryCoinDel.BindUFunction(this, FName(TEXT("SpawnSpecialCoin")), CherryCoinClass);
	GetWorld()->GetTimerManager().SetTimer(CherryCoinTimer, CherryCoinDel, 10.f, false);

}

void APMGameModeBase::AddPoints(int32 Points)
{
	Score += Points;

	if (HUDWidget != nullptr)
	{
		HUDWidget->SetScore(Score);
	}

	// If there are no more coins in the level
	if (APMCoin::GetCoinsNumber() == 0)
	{
		StopGame();
		HandleEndLevel(WinGameWidgetClass, WinGameSound, true);
	}

	if (bCoinSound && (CoinSound != nullptr))
	{
		bCoinSound = false;
		UGameplayStatics::PlaySound2D(GetWorld(), CoinSound);
	}
	else
	{
		bCoinSound = true;
	}
}

void APMGameModeBase::HandleGhostHit()
{
	StopGame();

	Lives--;
	if (Lives == 0)
	{
		HandleEndLevel(LoseGameWidgetClass, LoseGameSound, false);
		return;
	}

	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateLives(Lives, ESlateVisibility::Hidden);
	}

	if (PlayerHitSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerHitSound);
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

void APMGameModeBase::CreateEndGameWidget(const TSubclassOf<UPMEndGameWidget>& EndGameWidgetClass, int32 InScore, int32 InCherries)
{
	if (EndGameWidgetClass != nullptr)
	{
		UPMEndGameWidget* EndGameWidget = CreateWidget<UPMEndGameWidget>(GetWorld(), EndGameWidgetClass);
		EndGameWidget->OnBackToMenu.BindUObject(this, &APMGameModeBase::GoToMenu);
		EndGameWidget->OnRestartGame.BindUObject(this, &APMGameModeBase::RestartGameType);
		EndGameWidget->SetFinalScores(InScore, InCherries);
		EndGameWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::CreateEndGameWidget | EndGameWidgetClass is nullptr"));
	}
}

void APMGameModeBase::CreateNextLevelWidget()
{
	if (NextLevelWidgetClass != nullptr)
	{
		UPMNextLevelWidget* NextLevelWidget = CreateWidget<UPMNextLevelWidget>(GetWorld(), NextLevelWidgetClass);
		NextLevelWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::CreateNextLevelWidget | NextLevelWidgetClass is nullptr"));
	}
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

void APMGameModeBase::HandleEndLevel(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, USoundWave* EndGameSound, bool bWonGame)
{
	// Used when the game was lost or there was no next level.
	GameInstance->AddScoreboardData(GameInstance->GetCurrentLevelType(), FScoreboardData(Score, Cherries));
	GameInstance->SaveGame();
	
	if (EndGameSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EndGameSound);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());		
	}

	CreateEndGameWidget(EndGameWidgetClass, Score, Cherries);
}

void APMGameModeBase::OpenNextLevel(const FName& LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void APMGameModeBase::GoToMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Menu")));
}

void APMGameModeBase::SpawnSpecialCoin(const TSubclassOf<APMCoin>& SpecialCoinClass)
{
	const int32 RandomIndex = FMath::RandRange(0, Splines.Num() - 1);

	if (APMSpline* ChosenSpline = Cast<APMSpline>(Splines[RandomIndex]))
	{
		const float SplineLength = ChosenSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1);
		const FVector Location = ChosenSpline->SplineComponent->GetLocationAtDistanceAlongSpline(SplineLength / 2, ESplineCoordinateSpace::World);
		FRotator Rotation;
		UPMGameInstance::GetCurrentLevelType() == ELevelType::CLASSIC ? Rotation = FRotator(0,0,0) : Rotation = Splines[RandomIndex]->GetActorRotation();
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APMCoin>(SpecialCoinClass, Location, Rotation, SpawnInfo);
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

void APMGameModeBase::InitStartingWidgets()
{
	if (StarterWidgetClass != nullptr)
	{
		UPMStarterWidget* StarterWidget = CreateWidget<UPMStarterWidget>(GetWorld(), StarterWidgetClass);
		StarterWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitStartingWidgets | StarterWidgetClass is nullptr"));
	}

	if (PauseWidgetClass != nullptr)
	{
		PauseWidget = CreateWidget<UPMPauseWidget>(GetWorld(), PauseWidgetClass);
		PauseWidget->OnBackToMenu.BindUObject(this, &APMGameModeBase::GoToMenu);
		PauseWidget->OnContinue.BindUObject(this, &APMGameModeBase::ClosePauseMenu);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitStartingWidgets | WindGameWidgetClass is nullptr"));
	}
}

void APMGameModeBase::SetSplines()
{
	TArray<AActor*> OutSplines;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APMSpline::StaticClass(), OutSplines);
	for (const auto& Item : OutSplines)
	{
		if (!Item->ActorHasTag(FName(TEXT("withoutCoins"))))
		{
			Splines.Add(Item);
		}
	}
}

