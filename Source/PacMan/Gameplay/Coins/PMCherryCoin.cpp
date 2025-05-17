// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMCherryCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


int32 APMCherryCoin::Interaction()
{
	if (APMGameModeBase* GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (PickUpSound != nullptr) 
		{
			UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
		}
		GameMode->AddCherryCoin();
	}
	Destroy();
	return 100;
}


