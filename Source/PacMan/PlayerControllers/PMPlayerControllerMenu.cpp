// Fill out your copyright notice in the Description page of Project Settings.


#include "PMPlayerControllerMenu.h"
#include "Gameplay/Player/PMClassicPlayer.h"
#include "Gameplay/Player/PMMazePlayer.h"

void APMPlayerControllerMenu::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
	
}
