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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent)
	void AddPoints(int32 points);

	UFUNCTION(BlueprintNativeEvent)
	void HandleGhostHit();

	void StartGame();
	void StopGame();

	UFUNCTION(BlueprintImplementableEvent)
	void LostGame();

	UFUNCTION(BlueprintImplementableEvent)
	void WonGame();

	UFUNCTION(BlueprintCallable)
	void RestartGame();

	void AddCoin();
	void SubtractCoin();

	void PlayerAttackState();

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 Lives = 3;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APMGhost> GhostClass;

	int32 NumberOfCoins = 0;

	APMPlayer* Player = nullptr;
	TArray<APMGhost*> Ghosts;

};
