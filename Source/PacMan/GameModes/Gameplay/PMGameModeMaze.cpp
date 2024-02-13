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

void APMGameModeMaze::HideMap()
{
	if (UPMMazeHUD* mazeHUD = Cast<UPMMazeHUD>(HUDWidget))
	{
		mazeHUD->HideMap();
		bIsMapOpen = false;
		UE_LOG(LogTemp, Warning, TEXT("closeMap"));
	}
}

void APMGameModeMaze::UpdateMaps()
{
	if (bIsMapOpen || MapsNumber == 0) return;

	if (UPMMazeHUD* mazeHUD = Cast<UPMMazeHUD>(HUDWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("openMap"));
		mazeHUD->ShowMap();
		bIsMapOpen = true;
		mazeHUD->UpdateMapIcon(MapsNumber);
		MapsNumber--;

		FTimerHandle MapTimer;
		GetWorld()->GetTimerManager().SetTimer(MapTimer, this, &APMGameModeMaze::HideMap, 5.f, false);
	}
}

