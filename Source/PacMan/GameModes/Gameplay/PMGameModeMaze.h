// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMGameModeBase.h"
#include "PMGameModeMaze.generated.h"

class UPMMazeHUD;
class APMMapCoin;
class APMLifeCoin;
class USoundWave;
class UAudioComponent;

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeMaze : public APMGameModeBase
{
	GENERATED_BODY()

public:
	//Default constructor
	APMGameModeMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void HandleGhostHit() override;
	virtual void StartPlayerAttackState() override;
	virtual void EndPlayerAttackState() override;

	void HideMap();
	void ShowMap();
	void AddMap();
	int32 GetMapsNumber() { return MapsNumber; }

	void AddLife();
	int32 GetLivesNumber() { return Lives; }

protected:	
	virtual void InitStartingWidgets(APlayerController* PC) override;	
	virtual void HandleEndGame(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, USoundWave* EndGameSound, bool bWonGame) override;
	virtual void PlayerChasedHandle(bool IsPlayerChased) override;
	virtual void RestartGameType() override;
	virtual void SetGameplayValues() override;

private:
	void ClearChasedState();

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMMazeHUD> MazeHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMMapCoin> MapCoinClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMLifeCoin> LifeCoinClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> VulnerableSound;
	UPROPERTY()
	TObjectPtr<UPMMazeHUD> MazeHUD;
	UPROPERTY()
	TObjectPtr<UAudioComponent> PlayerAttackAC;

	int32 MapsNumber;
	bool bMapOpen;
	TArray<bool> ChasingGhosts;

	// Timers
	FTimerHandle VulnerableGhostTimer;
	FTimerHandle MapCoinTimer;
	FTimerHandle MapTimer;
	FTimerDelegate MapCoinDel;
	FTimerHandle LifeCoinTimer;
	FTimerDelegate LifeCoinDel;
	//
};
