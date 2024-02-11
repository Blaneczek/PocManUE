// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PMGameModeMenu.generated.h"

class UPMGameInstance;
class UPMMenuWidget;
enum class ELevelType : uint8 ;

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeMenu : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void ChooseGame(ELevelType GameType);
	UFUNCTION()
	void ContinueGame(ELevelType GameType);
	UFUNCTION()
	void ExitGame();

private:
	void InitializeMenu();

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UPMMenuWidget> MenuWidget;

private:
	UPROPERTY()
	TObjectPtr<UPMGameInstance> GameInstance;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets");
	TSubclassOf<UPMMenuWidget> MenuWidgetClass;

	
};
