// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeBase.generated.h"


class APMPlayer;
class APMGhost;

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

	UFUNCTION(BlueprintNativeEvent)
	void HandleGhostHit();

	void StartGame();
	void EndGame();

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 HealthPoints = 3;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APMGhost> GhostClass;
};
