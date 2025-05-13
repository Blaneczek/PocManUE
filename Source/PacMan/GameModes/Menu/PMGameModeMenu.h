// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeMenu.generated.h"

enum class ELevelType : uint8;
class UPMGameInstance;
class UPMMenuWidget;
class USoundWave;

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeMenu : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ChooseNewGame(ELevelType GameType);
	UFUNCTION()
	void ContinueGame(ELevelType GameType);
	UFUNCTION()
	void ExitGame();

	void InitializeMenu();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets");
	TSubclassOf<UPMMenuWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Audio");
	TObjectPtr<USoundWave> MenuAudio;

private:
	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;

};
