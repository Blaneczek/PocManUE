// Fill out your copyright notice in the Description page of Project Settings.

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
#include "GameInstance/PMGameInstance.h"


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

	GameInstance = Cast<UPMGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::BeginPlay | GameInstance is nullptr"));
		return;
	}
	
	CurrentLevel = GameInstance->GetCurrentLevel();

	SetPlayer();
	SetGhosts();
	SetSplines();
	
	StartGame();
	InitializeWidgets(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	FTimerHandle StartTimer;
	GetWorld()->GetTimerManager().SetTimer(StartTimer, this, &APMGameModeBase::StartAllMovement, 3.f, false);

	CherryCoinDel.BindUFunction(this, FName("SpawnSpecialCoin"), CherryCoinClass);
	GetWorld()->GetTimerManager().SetTimer(CherryCoinTimer, CherryCoinDel, 10.f, false);
}

void APMGameModeBase::AddPoints(int32 points)
{
	Score += points;
	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateScore(Score);
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
	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateLife(Lives);
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

void APMGameModeBase::OpenPauseMenu()
{

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

void APMGameModeBase::SpawnSpecialCoin(TSubclassOf<APMCoin> CoinClass)
{
	const int32 randomIndex = FMath::RandRange(0, Splines.Num() - 1);

	if (Splines[randomIndex] != nullptr)
	{
		const float splineLength = Splines[randomIndex]->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1);
		const FVector location = Splines[randomIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(splineLength / 2, ESplineCoordinateSpace::World);
		const FRotator rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APMCoin>(CoinClass, location, rotation, SpawnInfo);
	}
}

void APMGameModeBase::AddCherryCoin()
{
	GetWorld()->GetTimerManager().SetTimer(CherryCoinTimer, CherryCoinDel, 10.f, false);

	CherryNumber++;
	if (HUDWidget != nullptr)
	{
		HUDWidget->UpdateCherry(CherryNumber);
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
		LoseGameWidget->OnBackToMenu.AddDynamic(this, &APMGameModeBase::GoToMenu);
		LoseGameWidget->OnRestartGame.AddDynamic(this, &APMGameModeBase::RestartGameType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | LoseGameWidgetClass is nullptr"));
	}

	if (WinGameWidgetClass != nullptr)
	{
		WinGameWidget = CreateWidget<UPMEndGameWidget>(PlayerController, WinGameWidgetClass);
		WinGameWidget->OnBackToMenu.AddDynamic(this, &APMGameModeBase::GoToMenu);
		WinGameWidget->OnRestartGame.AddDynamic(this, &APMGameModeBase::RestartGameType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | WindGameWidgetClass is nullptr"));
	}
}

void APMGameModeBase::PlayerChasedHandle(bool IsPlayerChased)
{
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

void APMGameModeBase::SetSplines()
{
	TArray<AActor*> cherrySplines;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APMSpline::StaticClass(), cherrySplines);
	for (auto& item : cherrySplines)
	{
		APMSpline* spline = Cast<APMSpline>(item);
		if (spline != nullptr && !spline->ActorHasTag(TEXT("withoutCoins")))
		{
			Splines.Add(spline);
		}
	}
}
