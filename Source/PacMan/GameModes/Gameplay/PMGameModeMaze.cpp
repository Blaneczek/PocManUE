// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "UI/HUD/PMMazeHUD.h"

APMGameModeMaze::APMGameModeMaze()
{
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}
}