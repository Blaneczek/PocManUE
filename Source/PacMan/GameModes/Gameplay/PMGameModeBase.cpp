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


void APMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SetPlayer();
	SetGhosts();
	SetCherrySplines();
	
	StartGame();
	InitializeWidgets();
	StartingTimer(3.f);

	FTimerHandle StartTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &APMGameModeBase::StartAllMovement, 3.f, false);

	GetWorld()->GetTimerManager().SetTimer(CherryTimerHandle, this, &APMGameModeBase::SpawnCherryCoin, 15.f, false);
}

void APMGameModeBase::AddPoints_Implementation(int32 points)
{
	Score += points;
	if (ClassicHUD != nullptr)
	{
		ClassicHUD->UpdateScore(Score);
	}

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
	if (ClassicHUD != nullptr)
	{
		ClassicHUD->UpdateLife(Lives);
	}

	if (Lives == 0)
	{
		LostGame();
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

void APMGameModeBase::RestartGameType()
{
	APMPlayerController* PC = Cast<APMPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC != nullptr)
	{
		PC->RestartLevel();
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

	NumberOfCherryCoins++;
	if (ClassicHUD != nullptr)
	{
		ClassicHUD->UpdateCherry(NumberOfCherryCoins);
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
