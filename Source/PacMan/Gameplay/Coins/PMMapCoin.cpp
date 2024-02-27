// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMapCoin.h"
#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "Kismet/GameplayStatics.h"


int32 APMMapCoin::Interaction()
{
	if (APMGameModeMaze* GM = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(GetWorld())))
	{	
		// Interact only when the player does not have all the maps
		if (GM->GetMapsNumber() < 2)
		{			
			if (PickUpSound != nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
			}		
			Destroy();
		}
		GM->AddMap();
	}
	return 0;
}
