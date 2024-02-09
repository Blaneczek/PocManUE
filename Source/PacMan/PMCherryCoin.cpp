// Fill out your copyright notice in the Description page of Project Settings.


#include "PMCherryCoin.h"
#include "PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PMSpline.h"


int32 APMCherryCoin::Interaction()
{
	APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (gameMode != nullptr)
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


