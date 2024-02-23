// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeBase.generated.h"

class APMPlayer;
class APMGhost;
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
	//Default constructor
	APMGameModeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void HandleGhostHit();
	virtual void EndPlayerAttackState() {};
	virtual void StartPlayerAttackState();

	void SetPlayerChased(bool IsPlayerChased);
	void AddCherryCoin();
	void AddPoints(int32 points);
	void StopAllMovement();
	void OpenPauseMenu();

protected:	
	virtual void InitializeWidgets(APlayerController* PlayerController);
	virtual void SetGameplayValues() {};
	virtual void PlayerChasedHandle(bool IsPlayerChased) {};
	virtual void EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame);
	virtual void StopGame();
	virtual void StartAllMovement();
	virtual void RestartGameType() {};

	void StartGame();
	
	void GoToMenu();
	void ClosePauseMenu();

	UFUNCTION()
	void OpenNextLevel(const FName& LevelName);
	
	UFUNCTION()
	void SpawnSpecialCoin(TSubclassOf<APMCoin> SpecialCoinClass);

	void SetSplines();

public:
	// Gameplay variables
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Score;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Lives;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Cherries;
	//

	UPROPERTY()
	ELevelType CurrentLevelType;
	
	// Gameplay delegates
	FOnStartGame OnStartGame;
	FOnStopGame OnStopGame;
	FOnStartMovement OnStartMovement;
	FOnStopMovement OnStopMovement;
	FOnPlayerAttack OnPlayerAttack;
	//

protected:
	// Widgets
	UPROPERTY(BlueprintReadWrite, Category ="PocMan|Widgets")
	TObjectPtr<UPMHUDWidget> HUDWidget;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMStarterWidget> StarterWidget;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMEndGameWidget> LoseGameWidget;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMEndGameWidget> WinGameWidget;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMPauseWidget> PauseWidget;
	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMNextLevelWidget> NextLevelWidget;
	//
	 
	// Widget classes
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
	//

	UPROPERTY()
	TArray<TObjectPtr<APMSpline>> Splines;
	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;
	
	UPROPERTY()
	int32 CurrentLevelNum;

	// Audio
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
	//
	
	// Gameplay classes
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMGhost> GhostClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMCoin> CoinClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMCherryCoin> CherryCoinClass;
	//


	//Timers
	FTimerHandle CherryCoinTimer;
	FTimerDelegate CherryCoinDel;
	FTimerHandle StartMovementTimerHandle;
	FTimerHandle ResetGameTimer;
	FTimerHandle StartMovementTimer;
	// used to use the sound of every second coin collected
	bool bCoinSound;
};
