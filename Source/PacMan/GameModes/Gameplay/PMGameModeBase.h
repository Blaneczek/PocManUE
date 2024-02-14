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
class UPMGameInstance;
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
	virtual void PlayerChasedHandle(bool IsPlayerChased);
	virtual void HandleEndGame(UPMEndGameWidget* EndGameWidget);

public:
	virtual void HandleGhostHit();
	void AddPoints(int32 points);
	void StartGame();
	void StopGame();
	void StopAllMovement();
	void StartAllMovement();
	
	void OpenPauseMenu();

	UFUNCTION()
	void RestartGameType();

	UFUNCTION()
	void GoToMenu();

	void AddCoin();
	void SubtractCoin();

	UFUNCTION()
	void SpawnSpecialCoin(TSubclassOf<APMCoin> CoinClass);

	void AddCherryCoin();

	virtual void PlayerAttackState();
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
	
	// Widgets
	UPROPERTY(BlueprintReadWrite, Category ="PocMan|Widgets")
	TObjectPtr<UPMHUDWidget> HUDWidget;

	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMStarterWidget> StarterWidget;

	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMEndGameWidget> LoseGameWidget;

	UPROPERTY(BlueprintReadWrite, Category = "PocMan|Widgets")
	TObjectPtr<UPMEndGameWidget> WinGameWidget;
	//

	UPROPERTY()
	TArray<TObjectPtr<APMSpline>> Splines;

private:
	// Classes
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMEndGameWidget> LoseGameWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMEndGameWidget> WinGameWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMStarterWidget> StarterWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMGhost> GhostClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	TSubclassOf<APMCherryCoin> CherryCoinClass;
	//
	
	// Timers
	FTimerHandle CherryCoinTimer;
	FTimerDelegate CherryCoinDel;
	//

	UPROPERTY()
	TObjectPtr<APMPlayer> Player;
	UPROPERTY()
	TArray<TObjectPtr<APMGhost>> Ghosts;
	
	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;
	
	ELevelType CurrentLevel;
	int32 NumberOfCoins;
};
