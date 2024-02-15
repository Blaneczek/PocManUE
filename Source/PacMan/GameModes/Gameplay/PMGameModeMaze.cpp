// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "UI/HUD/PMMazeHUD.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "Gameplay/Coins/PMMapCoin.h"

APMGameModeMaze::APMGameModeMaze()
{
	MapsNumber = 2;
	bIsMapOpen = false;
	bIsPlayerAlreadyChased = false;
	bIsStillVulnerable = false;
}

void APMGameModeMaze::BeginPlay()
{
	Super::BeginPlay();

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

	if (bIsStillVulnerable && VulnerableScreenTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableScreenTimer);
		MazeHUD->ShowVulnerableScreen();
		GetWorld()->GetTimerManager().SetTimer(VulnerableScreenTimer, this, &APMGameModeMaze::HideVulnerableScreen, 7.f, false);
		bIsStillVulnerable = true;
		return;
	}

	MazeHUD->ShowVulnerableScreen();	
	GetWorld()->GetTimerManager().SetTimer(VulnerableScreenTimer, this, &APMGameModeMaze::HideVulnerableScreen, 7.f, false);
	bIsStillVulnerable = true;
}

void APMGameModeMaze::HideMap()
{
	MazeHUD->HideMap();
	bIsMapOpen = false;
}

void APMGameModeMaze::ShowMap()
{
	if (bIsMapOpen || MapsNumber == 0) return;

	MazeHUD->ShowMap();
	bIsMapOpen = true;
	MazeHUD->UpdateMapIcon(MapsNumber, ESlateVisibility::Hidden);
	MapsNumber--;

	FTimerHandle MapTimer;
	GetWorld()->GetTimerManager().SetTimer(MapTimer, this, &APMGameModeMaze::HideMap, 5.f, false);
}

void APMGameModeMaze::AddMap()
{
	if (MapsNumber == 2) return;

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
}

void APMGameModeMaze::ClearChasedState()
{
	bIsStillVulnerable = false;
	ChasingGhosts.Empty();
	MazeHUD->HideChaseScreen();
}

void APMGameModeMaze::HideVulnerableScreen()
{
	if (VulnerableScreenTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableScreenTimer);
	}
	bIsStillVulnerable = false;
	MazeHUD->HideVulnerableScreen();
}

