// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeMenu.generated.h"

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
	UPROPERTY(BlueprintReadWrite, Category = "PocMan")
	TObjectPtr<UPMMenuWidget> MenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets");
	TSubclassOf<UPMMenuWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Audio");
	TObjectPtr<USoundWave> MenuAudio;

	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;

	

	
};
