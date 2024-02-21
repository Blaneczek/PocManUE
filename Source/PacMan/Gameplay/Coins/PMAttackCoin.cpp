// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMAttackCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


int32 APMAttackCoin::Interaction()
{
	if (APMGameModeBase* GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->PlayerAttackState();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMAttackCoin::Interaction | gameMode is nullptr"))
	}

	CoinsCounter--;
	Destroy();
	return 10;
}
