// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMEndGameWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DELEGATE(FBackToMenu);
DECLARE_DELEGATE(FRestartCurrentGame);

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
	void SetFinalScores(int32 Score, int32 Cherries);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> MenuButton;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UTextBlock> FinalScoresText;

public:
	FBackToMenu OnBackToMenu;
	FRestartCurrentGame OnRestartGame;
};
