// Fill out your copyright notice in the Description page of Project Settings.

#include "PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllers/PMPlayerController.h"
#include "Gameplay/Player/PMPlayer.h"
#include "Gameplay/Ghosts/PMGhost.h"
#include "Gameplay/Coins/PMCherryCoin.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "UI/HUD/PMClassicHUD.h"
#include "UI/HUD/PMEndGameWidget.h"
#include "UI/HUD/PMStarterWidget.h"


APMGameModeBase::APMGameModeBase()
{
	Score = 0;
	NumberOfCoins = 0;
	Lives = 3;
	CherryNumber = 0;
}

void APMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SetPlayer();
	SetGhosts();
	SetCherrySplines();
	
	StartGame();
	InitializeWidgets();

	FTimerHandle StartTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &APMGameModeBase::StartAllMovement, 3.f, false);

	GetWorld()->GetTimerManager().SetTimer(CherryTimerHandle, this, &APMGameModeBase::SpawnCherryCoin, 15.f, false);
}

void APMGameModeBase::AddPoints(int32 points)
{
	Score += points;
	if (ClassicHUD != nullptr)
	{
		ClassicHUD->UpdateScore(Score);
	}

	if (NumberOfCoins == 0)
	{
		StopGame();
		HandleEndGame(WinGameWidget);
	}
}

void APMGameModeBase::HandleGhostHit()
{
	StopGame();
	Lives--;
	if (ClassicHUD != nullptr)
	{
		ClassicHUD->UpdateLife(Lives);
	}

	if (Lives == 0)
	{
		HandleEndGame(LoseGameWidget);
		return;
	}

	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMGameModeBase::StartGame, 2.f, false);
	FTimerHandle StartTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &APMGameModeBase::StartAllMovement, 3.f, false);
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

void APMGameModeBase::HandleEndGame(UPMEndGameWidget* EndGameWidget)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC == nullptr) return;

	PC->SetShowMouseCursor(true);
	PC->SetInputMode(FInputModeUIOnly());

	if (EndGameWidget != nullptr)
	{
		EndGameWidget->AddToViewport();
	}
}

void APMGameModeBase::RestartGameType()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC != nullptr)
	{
		PC->RestartLevel();
	}	
}

void APMGameModeBase::GoToMenu()
{
	UGameplayStatics::OpenLevel(this, "Menu");
}

void APMGameModeBase::AddCoin()
{
	NumberOfCoins++;
}

void APMGameModeBase::SubtractCoin()
{
	NumberOfCoins--;
}

void APMGameModeBase::SpawnCherryCoin()
{
	const int32 randomIndex = FMath::RandRange(0, CherrySplines.Num() - 1);

	if (CherrySplines[randomIndex] != nullptr)
	{
		const float splineLength = CherrySplines[randomIndex]->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1);
		const FVector location = CherrySplines[randomIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(splineLength / 2, ESplineCoordinateSpace::World);
		const FRotator rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APMCoin>(CherryCoinClass, location, rotation, SpawnInfo);
	}
}

void APMGameModeBase::AddCherryCoin()
{
	GetWorld()->GetTimerManager().SetTimer(CherryTimerHandle, this, &APMGameModeBase::SpawnCherryCoin, 10.f, false);

	CherryNumber++;
	if (ClassicHUD != nullptr)
	{
		ClassicHUD->UpdateCherry(CherryNumber);
	}
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

void APMGameModeBase::InitializeWidgets()
{
	APMPlayerController* PC = Cast<APMPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | PlayerController is nullptr"));
		return;
	}

	if (ClassicHUDClass != nullptr)
	{
		ClassicHUD = CreateWidget<UPMClassicHUD>(PC, ClassicHUDClass);
		ClassicHUD->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}

	if (StarterWidgetClass != nullptr)
	{
		StarterWidget = CreateWidget<UPMStarterWidget>(PC, StarterWidgetClass);
		StarterWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | StarterWidgetClass is nullptr"));
	}

	if (LoseGameWidgetClass != nullptr)
	{
		LoseGameWidget = CreateWidget<UPMEndGameWidget>(PC, LoseGameWidgetClass);
		LoseGameWidget->OnBackToMenu.AddDynamic(this, &APMGameModeBase::GoToMenu);
		LoseGameWidget->OnRestartGame.AddDynamic(this, &APMGameModeBase::RestartGameType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | LoseGameWidgetClass is nullptr"));
	}

	if (WinGameWidgetClass != nullptr)
	{
		WinGameWidget = CreateWidget<UPMEndGameWidget>(PC, WinGameWidgetClass);
		WinGameWidget->OnBackToMenu.AddDynamic(this, &APMGameModeBase::GoToMenu);
		WinGameWidget->OnRestartGame.AddDynamic(this, &APMGameModeBase::RestartGameType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | WindGameWidgetClass is nullptr"));
	}
}

void APMGameModeBase::SetPlayer()
{
	Player = Cast<APMPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::SetPlayer | Player is nullptr"));
	}
}

void APMGameModeBase::SetGhosts()
{
	TArray<AActor*> ghosts;
	UGameplayStatics::GetAllActorsOfClass(this, GhostClass, ghosts);
	for (auto& item : ghosts)
	{
		APMGhost* ghost = Cast<APMGhost>(item);
		if (ghost != nullptr)
		{
			Ghosts.Add(ghost);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::SetGhosts | Ghost is nullptr"));
		}
	}
}

void APMGameModeBase::SetCherrySplines()
{
	TArray<AActor*> cherrySplines;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), FName(TEXT("cherry")), cherrySplines);
	for (auto& item : cherrySplines)
	{
		APMSpline* spline = Cast<APMSpline>(item);
		if (spline != nullptr)
		{
			CherrySplines.Add(spline);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::SetCherrySplines | Spline is nullptr"));
		}
	}
}
