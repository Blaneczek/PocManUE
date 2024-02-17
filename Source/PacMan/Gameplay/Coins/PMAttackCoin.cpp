// Fill out your copyright notice in the Description page of Project Settings.

#include "PMAttackCoin.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Kismet/GameplayStatics.h"


void APMAttackCoin::SetMaterial()
{
}

int32 APMAttackCoin::Interaction()
{
	if (APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
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
