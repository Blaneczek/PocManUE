// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMGameModeBase.h"
#include "PMGameModeClassic.generated.h"


class UPMClassicHUD;
class UAudioComponent;

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeClassic : public APMGameModeBase
{
	GENERATED_BODY()

public:
	//Default constructor
	APMGameModeClassic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void StartPlayerAttackState() override;
	virtual void EndPlayerAttackState() override;

protected:
	virtual void InitStartingWidgets() override;
	virtual void HandleEndLevel(TSubclassOf<UPMEndGameWidget> EndGameWidgetClass, USoundWave* EndGameSound, bool bWonGame) override;
	virtual void SetGameplayValues() override;
	virtual void StartAllMovement() override;
	virtual void StopGame() override;
	virtual void RestartGameType() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMClassicHUD> ClassicHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> GhostSound;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> VulnerableGhostSound;
	UPROPERTY()
	TObjectPtr<UAudioComponent> GhostAudio;
};
