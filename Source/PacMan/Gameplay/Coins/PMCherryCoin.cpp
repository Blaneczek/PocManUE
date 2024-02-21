// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMCherryCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


int32 APMCherryCoin::Interaction()
{
	if (APMGameModeBase* GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->AddCherryCoin();

		if (PickUpSound != nullptr) UGameplayStatics::PlaySound2D(this, PickUpSound);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMCherryCoin::Interaction | gameMode is nullptr"))
	}
	Destroy();
	return 100;
}


