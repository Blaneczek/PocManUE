// Fill out your copyright notice in the Description page of Project Settings.


#include "PMGameModeBase.h"

void APMGameModeBase::AddPoints_Implementation(int32 points)
{
	Score += points;
	//update widget
}

void APMGameModeBase::HandleGhostHit()
{
	HealthPoints--;
	if (HealthPoints == 0)
	{
		EndGame();
		return;
	}
}

void APMGameModeBase::EndGame()
{
}
