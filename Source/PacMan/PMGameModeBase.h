// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent)
	void AddPoints(int32 points);

	void HandleGhostHit();

	void EndGame();

public:

	UPROPERTY(BlueprintReadWrite)
	int32 Score = 0;

	int32 HealthPoints = 3;
};
