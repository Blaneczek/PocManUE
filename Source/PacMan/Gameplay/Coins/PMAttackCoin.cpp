// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMAttackCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


int32 APMAttackCoin::Interaction()
{
	if (APMGameModeBase* GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->StartPlayerAttackState();
	}
	CoinsCounter--;
	Destroy();
	return 10;
}
