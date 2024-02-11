// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeBase.generated.h"


class APMPlayer;
class APMGhost;
class APMSpline;
class APMCherryCoin;
class UPMClassicHUD;

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
	void StopAllMovement();
	void StartAllMovement();

	UFUNCTION(BlueprintImplementableEvent)
	void LostGame();

	UFUNCTION(BlueprintImplementableEvent)
	void WonGame();

	UFUNCTION(BlueprintImplementableEvent)
	void StartingTimer(float time);

	UFUNCTION(BlueprintCallable)
	void RestartGameType();

	void AddCoin();
	void SubtractCoin();
	void SpawnCherryCoin();
	void AddCherryCoin();

	void PlayerAttackState();

private:
	void InitializeWidgets();
	void SetPlayer();
	void SetGhosts();
	void SetCherrySplines();

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 Lives = 3;

	UPROPERTY(BlueprintReadWrite)
	int32 NumberOfCherryCoins = 0;

	UPROPERTY(BlueprintReadWrite, Category ="PocMan|Widgets")
	UPMClassicHUD* ClassicHUD;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPMClassicHUD> ClassicHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APMGhost> GhostClass;

	int32 NumberOfCoins = 0;

	APMPlayer* Player = nullptr;
	TArray<APMGhost*> Ghosts;
	TArray<APMSpline*> CherrySplines;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APMCherryCoin> CherryCoinClass;

	FTimerHandle CherryTimerHandle;
};
