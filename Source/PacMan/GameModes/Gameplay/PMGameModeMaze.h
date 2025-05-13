// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMGameModeBase.h"
#include "PMGameModeMaze.generated.h"

class UPMMazeHUD;
class APMMapCoin;
class APMLifeCoin;
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

	/** Shows the chase screen as long as there is any ghost chasing the player */
	virtual void SetPlayerChased(bool IsPlayerChased) override;

	void HideMap();
	void ShowMap();
	void AddMap();
	int32 GetMapsNumber() const { return MapsNumber; } 

	void AddLife();
	int32 GetLivesNumber() const { return Lives; }

protected:	
	virtual void InitStartingWidgets() override;	
	virtual void HandleEndGame(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, USoundWave* EndGameSound, bool bWonGame) override;
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

	// Counter of chasing Ghosts for displaying Chase screen effect on HUD
	int32 ChasingGhostsCount;
	
	FTimerHandle MapCoinTimer;
	FTimerHandle MapTimer;
	FTimerDelegate MapCoinDel;
	FTimerHandle LifeCoinTimer;
	FTimerDelegate LifeCoinDel;
};
