// Fill out your copyright notice in the Description page of Project Settings.


#include "PMCherryCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Splines/PMSpline.h"


int32 APMCherryCoin::Interaction()
{
	if (APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		gameMode->AddCherryCoin();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMCherryCoin::Interaction | gameMode is nullptr"))
	}
	Destroy();
	return 100;
}


