// Fill out your copyright notice in the Description page of Project Settings.

#include "PMAttackCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


int32 APMAttackCoin::Interaction()
{
	APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (gameMode != nullptr)
	{
		gameMode->PlayerAttackState();
		UE_LOG(LogTemp, Warning, TEXT("AttackCoin"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMAttackCoin::Interaction | gameMode is nullptr"))
	}
	Destroy();
	return 20;
}
