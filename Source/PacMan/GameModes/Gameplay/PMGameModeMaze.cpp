// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "UI/HUD/PMMazeHUD.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "Gameplay/Coins/PMMapCoin.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Ghosts/PMGhost.h"
#include "Components/AudioComponent.h"

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
	Super::InitializeWidgets(PlayerController);

	if (MazeHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMMazeHUD>(PlayerController, MazeHUDClass);
		HUDWidget->AddToViewport();
		MazeHUD = Cast<UPMMazeHUD>(HUDWidget);
		if (MazeHUD == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | MazeHUD is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}
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

void APMGameModeMaze::EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound)
{
	Super::EndGameHandle(EndGameWidget, EndGameSound);

	ClearChasedState();
}

void APMGameModeMaze::HandleGhostHit()
{
	Super::HandleGhostHit();

	ClearChasedState();
}

void APMGameModeMaze::PlayerAttackState()
{
	Super::PlayerAttackState();

	ClearChasedState();

	if (bStillVulnerable && VulnerableScreenTimer.IsValid())
	{
		if (IsValid(PlayerAttackAC))
		{
			PlayerAttackAC->Stop();
			PlayerAttackAC = UGameplayStatics::SpawnSound2D(this, VulnerableSound);
		}
		GetWorld()->GetTimerManager().ClearTimer(VulnerableScreenTimer);
		GetWorld()->GetTimerManager().SetTimer(VulnerableScreenTimer, this, &APMGameModeMaze::EndPlayerAttackState, 7.f, false);
		bStillVulnerable = true;
		return;
	}

	PlayerAttackAC = UGameplayStatics::SpawnSound2D(this, VulnerableSound);
	MazeHUD->ShowVulnerableScreen();	
	GetWorld()->GetTimerManager().SetTimer(VulnerableScreenTimer, this, &APMGameModeMaze::EndPlayerAttackState, 7.f, false);
	bStillVulnerable = true;
}

void APMGameModeMaze::EndPlayerAttackState()
{
	if (VulnerableScreenTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableScreenTimer);
	}
	if (IsValid(PlayerAttackAC))
	{
		PlayerAttackAC->Stop();
	}
	bStillVulnerable = false;
	MazeHUD->HideVulnerableScreen();
}

void APMGameModeMaze::HideMap()
{
	MazeHUD->HideMap();
	bMapOpen = false;
}

void APMGameModeMaze::ShowMap()
{
	if (bMapOpen || MapsNumber == 0) return;

	MazeHUD->ShowMap();
	bMapOpen = true;
	MazeHUD->UpdateMapIcon(MapsNumber, ESlateVisibility::Hidden);
	MapsNumber--;

	FTimerHandle MapTimer;
	GetWorld()->GetTimerManager().SetTimer(MapTimer, this, &APMGameModeMaze::HideMap, 5.f, false);
}

void APMGameModeMaze::AddMap()
{
	if (MapsNumber == 2)
	{
		//TODO: mazeHud-> add text "can't add more maps"
		return;
	}
		

	if (MapsNumber == 1)
	{
		MazeHUD->UpdateMapIcon(2, ESlateVisibility::Visible);
		MapsNumber++;
	}
	else if (MapsNumber == 0)
	{
		MazeHUD->UpdateMapIcon(1, ESlateVisibility::Visible);
		MapsNumber++;
	}

	GetWorld()->GetTimerManager().SetTimer(MapCoinTimer, MapCoinDel, 15.f, false);
}

void APMGameModeMaze::ClearChasedState()
{
	ChasingGhosts.Empty();
	MazeHUD->HideChaseScreen();
}

