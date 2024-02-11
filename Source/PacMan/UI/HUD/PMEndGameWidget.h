// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMEndGameWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBackToMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRestartCurrentGame);

class UButton;

/**
 * 
 */
UCLASS()
class PACMAN_API UPMEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

	UFUNCTION()
	void OnMenuButtonClicked();
	UFUNCTION()
	void OnRestartButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* MenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

public:
	UPROPERTY()
	FBackToMenu OnBackToMenu;
	
	UPROPERTY()
	FRestartCurrentGame OnRestartGame;
};
