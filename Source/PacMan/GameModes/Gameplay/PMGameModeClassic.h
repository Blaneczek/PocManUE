// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMGameModeBase.h"
#include "PMGameModeClassic.generated.h"


class UPMClassicHUD;
class USoundWave;
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

	virtual void InitializeWidgets(APlayerController* PlayerController) override;
	virtual void EndGameHandle(UPMEndGameWidget* EndGameWidget, USoundWave* EndGameSound, bool bWonGame);
	virtual void SetGameplayValues() override;
	virtual void StartAllMovement() override;
	virtual void StopGame() override;
	virtual void RestartGameType() override;

public:
	virtual void PlayerAttackState() override;
	virtual void EndPlayerAttackState() override;

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
