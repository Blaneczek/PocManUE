// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMEndGameWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBackToMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRestartCurrentGame);

/**
 * 
 */
UCLASS()
class PACMAN_API UPMEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMenuButtonClicked();
	UFUNCTION()
	void OnRestartButtonClicked();

public:
	void SetScore(int32 Score, int32 Cherries);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

public:
	UPROPERTY()
	FBackToMenu OnBackToMenu;
	UPROPERTY()
	FRestartCurrentGame OnRestartGame;
};
