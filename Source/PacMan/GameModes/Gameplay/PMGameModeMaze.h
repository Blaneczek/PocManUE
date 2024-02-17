// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "PMGameModeMaze.generated.h"

class UPMMazeHUD;
class APMMapCoin;
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

	virtual void InitializeWidgets(APlayerController* PlayerController) override;	
	virtual void EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame) override;
	virtual void PlayerChasedHandle(bool IsPlayerChased) override;
	virtual void RestartGameType() override;
	virtual void SetGameplayValues() override;

public:
	virtual void HandleGhostHit() override;
	virtual void PlayerAttackState() override;
	virtual void EndPlayerAttackState() override;

	void HideMap();
	void ShowMap();
	void AddMap();
	int32 GetMapsNumber() { return MapsNumber; }

private:
	void ClearChasedState();

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMMazeHUD> MazeHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<APMMapCoin> MapCoinClass;

	UPROPERTY()
	TObjectPtr<UPMMazeHUD> MazeHUD;

	UPROPERTY()
	TObjectPtr<UAudioComponent> PlayerAttackAC;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> VulnerableSound;

	int32 MapsNumber;
	bool bMapOpen;
	bool bPlayerAlreadyChased;
	bool bStillVulnerable;

	TArray<bool> ChasingGhosts;

	// Timers
	FTimerHandle VulnerableScreenTimer;

	FTimerHandle MapCoinTimer;
	FTimerDelegate MapCoinDel;
	//
};
