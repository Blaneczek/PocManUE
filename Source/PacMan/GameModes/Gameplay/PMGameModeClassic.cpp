// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGameModeClassic.h"
#include "UI/HUD/PMClassicHUD.h"

APMGameModeClassic::APMGameModeClassic()
{
}

void APMGameModeClassic::BeginPlay()
{
	Super::BeginPlay();
}

void APMGameModeClassic::InitializeWidgets(APlayerController* PlayerController)
{
	Super::InitializeWidgets(PlayerController);

	if (ClassicHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMClassicHUD>(PlayerController, ClassicHUDClass);
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}
}
