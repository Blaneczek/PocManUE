// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMapCoin.h"
#include "GameModes/Gameplay/PMGameModeMaze.h"
#include "Kismet/GameplayStatics.h"

int32 APMMapCoin::Interaction()
{
	if (APMGameModeMaze* GM = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(this)))
	{
		if (GM->GetMapsNumber() < 2)
		{
			if (PickUpSound != nullptr) UGameplayStatics::PlaySound2D(this, PickUpSound);

			GM->AddMap();
			Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APMMapCoin::Interaction | gameMode is nullptr"))
		}
	}
	return 0;
}
