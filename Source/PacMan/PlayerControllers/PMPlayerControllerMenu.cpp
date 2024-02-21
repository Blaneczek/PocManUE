// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMPlayerControllerMenu.h"

void APMPlayerControllerMenu::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());	
}
