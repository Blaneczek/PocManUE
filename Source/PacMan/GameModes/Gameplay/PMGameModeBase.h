// Fill out your copyright notice in the Description page of Project Settings.

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

	virtual void InitializeWidgets(APlayerController* PlayerController);
	virtual void SetGameplayValues() {};
	virtual void PlayerChasedHandle(bool IsPlayerChased);
	virtual void EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame);

	UFUNCTION()
	void OpenNextLevel(const FName& LevelName);
	UFUNCTION()
	virtual void RestartGameType() {};

public:
	virtual void HandleGhostHit();
	void AddPoints(int32 points);
	void StartGame();
	virtual void StopGame();
	void StopAllMovement();
	virtual void StartAllMovement();
	
	void OpenPauseMenu();
	UFUNCTION()
	void ClosePauseMenu();

	

	UFUNCTION()
	void GoToMenu();

	void AddCoin();
	void SubtractCoin();

	UFUNCTION()
	void SpawnSpecialCoin(TSubclassOf<APMCoin> CoinClass);

	void AddCherryCoin();

	virtual void PlayerAttackState();
	virtual void EndPlayerAttackState();
	void SetPlayerChased(bool IsPlayerChased);

private:
	
	void SetPlayer();
	void SetGhosts();
	void SetSplines();

public:
	// Gameplay variables
	UPROPERTY(BlueprintReadWrite)
	int32 Score;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 Lives;
	UPROPERTY(BlueprintReadWrite)
	int32 CherryNumber;
	//

	UPROPERTY()
	ELevelType CurrentLevelType;
	

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
	TArray<TObjectPtr<APMGhost>> Ghosts;
	UPROPERTY()
	TObjectPtr<APMPlayer> Player;	
	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;
	
	UPROPERTY()
	int32 CurrentLevelNum;

	// Sounds
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

	// Gameplay classes
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMGhost> GhostClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMCherryCoin> CherryCoinClass;

	
	
	// Timers
	FTimerHandle CherryCoinTimer;
	FTimerDelegate CherryCoinDel;
	//	


	int32 NumberOfCoins;
	bool bCoinSound;
};
