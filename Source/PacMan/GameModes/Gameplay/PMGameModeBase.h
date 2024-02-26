// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeBase.generated.h"

class APMSpline;
class APMCoin;
class APMCherryCoin;
class UPMHUDWidget;
class UPMEndGameWidget;
class UPMStarterWidget;
class UPMPauseWidget;
class UPMNextLevelWidget;
class UPMGameInstance;
class USoundWave;
enum class ELevelType : uint8;

DECLARE_MULTICAST_DELEGATE(FOnStartGame);
DECLARE_MULTICAST_DELEGATE(FOnStopGame);
DECLARE_MULTICAST_DELEGATE(FOnStartMovement);
DECLARE_MULTICAST_DELEGATE(FOnStopMovement);
DECLARE_MULTICAST_DELEGATE(FOnPlayerAttack);

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APMGameModeBase();

protected:
	virtual void BeginPlay() override;

public:
	/** When a player is hit by any ghost */
	virtual void HandleGhostHit();

	/** When a player collects attack coin */
	virtual void StartPlayerAttackState();
	virtual void EndPlayerAttackState() {};

	/** When any ghost sees the player or loses sight of the player */
	virtual void SetPlayerChased(bool IsPlayerChased) {};

	void AddCherryCoin();
	void AddPoints(int32 points);
	void StopAllMovement();
	void OpenPauseMenu();

protected:
	/** Initialize widgets that are added to viewport at the beggining or will be used many times later in the game */
	virtual void InitStartingWidgets();

	/** Set gameplay data from GameInstance depending on the game mode */
	virtual void SetGameplayValues() {};

	virtual void HandleEndGame(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, USoundWave* EndGameSound, bool bWonGame);
	virtual void StopGame();
	virtual void StartAllMovement();
	virtual void RestartGameType() {};

	void CreateEndGameWidget(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, int32 InScore, int32 InCherries);
	void CreateNextLevelWidget();

	void StartGame();
	
	void GoToMenu();
	void ClosePauseMenu();

	UFUNCTION()
	void OpenNextLevel(const FName& LevelName);
	
	UFUNCTION()
	void SpawnSpecialCoin(TSubclassOf<APMCoin> SpecialCoinClass);

	/** Populates the Spline array with pointers to the splines at the current level */
	void SetSplines();

public:
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Score;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Lives;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Cherries;

	/** Delegates to control player and ghosts */
	FOnStartGame OnStartGame;
	FOnStopGame OnStopGame;
	FOnStartMovement OnStartMovement;
	FOnStopMovement OnStopMovement;
	FOnPlayerAttack OnPlayerAttack;

	UPROPERTY()
	int32 CurrentLevelNum;

protected:
	/** Widgets that will be used many times during the game */
	UPROPERTY(BlueprintReadWrite, Category ="PocMan|Widgets")
	TObjectPtr<UPMHUDWidget> HUDWidget;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMPauseWidget> PauseWidget;

	/** Blueprint widget classes to be set in blueprint version of PMGameModeBase */
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMEndGameWidget> LoseGameWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMEndGameWidget> WinGameWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMStarterWidget> StarterWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMPauseWidget> PauseWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMNextLevelWidget> NextLevelWidgetClass;

	/** Table containing all the splines from the level to spawn special coins */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Splines;

	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;
	
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> GameMusic;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> WinGameSound;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> LoseGameSound;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> PlayerHittedSound;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> CoinSound;

	/** Blueprint coin classes are different depending on the game mode */
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMCoin> CoinClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMCherryCoin> CherryCoinClass;

	/** To spawn cherries at appropriate intervals  */
	FTimerHandle CherryCoinTimer;
	FTimerDelegate CherryCoinDel;

	/** To control the flow of the game with appropriate delays */
	FTimerHandle StartMovementTimerHandle;
	FTimerHandle ResetGameTimer;
	FTimerHandle StartMovementTimer;
	FTimerHandle VulnerableGhostTimer;

	/** used to make the sound of every second coin collected */
	bool bCoinSound;
};
