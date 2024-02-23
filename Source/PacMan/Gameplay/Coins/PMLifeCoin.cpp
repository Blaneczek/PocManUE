// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "Gameplay/Coins/PMLifeCoin.h"
#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "Kismet/GameplayStatics.h"

int32 APMLifeCoin::Interaction()
{
	if (APMGameModeMaze* GM = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->GetLivesNumber() < 3)
		{
			if (PickUpSound != nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
			}
			Destroy();
		}
		GM->AddLife();
	}
	return 0;
}
