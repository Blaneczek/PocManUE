// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "Gameplay/Coins/PMLifeCoin.h"
#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "Kismet/GameplayStatics.h"

int32 APMLifeCoin::Interaction()
{
	if (APMGameModeMaze* GameMode = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		// Interact only when the player does not have all the lives 
		if (GameMode->GetLivesNumber() < 3)
		{
			if (PickUpSound != nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
			}
			GameMode->AddLife();
			Destroy();
		}
	}
	return 0;
}
