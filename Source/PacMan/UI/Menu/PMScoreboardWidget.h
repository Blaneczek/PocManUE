// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMScoreboardWidget.generated.h"

class UButton;
class UMultiLineEditableTextBox;

/**
 * 
 */
UCLASS()
class PACMAN_API UPMScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetFinalScores();

protected:
	void NativeConstruct() override;

	UFUNCTION()
	void OnXButtonClicked();
	UFUNCTION()
	void OnClearButtonClicked();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> XScoreButton;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> ClearScoreButton;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> ClassicScore;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> MazeScore;
};
