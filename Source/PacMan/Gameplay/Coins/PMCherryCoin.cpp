// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMCherryCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


void APMCherryCoin::SetMaterial()
{
}

int32 APMCherryCoin::Interaction()
{
	if (APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		gameMode->AddCherryCoin();

		if (PickUpSound != nullptr) UGameplayStatics::PlaySound2D(this, PickUpSound);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMCherryCoin::Interaction | gameMode is nullptr"))
	}
	Destroy();
	return 100;
}


