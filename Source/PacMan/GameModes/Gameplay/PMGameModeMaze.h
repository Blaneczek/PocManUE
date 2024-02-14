// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "PMGameModeMaze.generated.h"

class UPMMazeHUD;

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
	virtual void PlayerChasedHandle(bool IsPlayerChased) override;
	virtual void HandleEndGame(UPMEndGameWidget* EndGameWidget) override;
	
public:
	virtual void HandleGhostHit() override;
	virtual void PlayerAttackState() override;

	void HideMap();
	void ShowMap();
	void AddMap();
	int32 GetMapsNumber() { return MapsNumber; }

private:
	void ClearChasedState();
	void HideVulnerableScreen();

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMMazeHUD> MazeHUDClass;

	UPROPERTY()
	TObjectPtr<UPMMazeHUD> MazeHUD;

	int32 MapsNumber;
	bool bIsMapOpen;
	bool bIsPlayerAlreadyChased;
	bool bIsStillVulnerable;

	TArray<bool> ChasingGhosts;

	FTimerHandle VulnerableScreenTimer;
};
