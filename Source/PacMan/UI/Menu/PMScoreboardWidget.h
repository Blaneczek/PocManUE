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

protected:
	void NativeConstruct() override;

	UFUNCTION()
	void OnXButtonClicked();

public:
	void SetScores();

protected:
	UPROPERTY(meta = (BindWidget))	
	TObjectPtr<UButton> XScoreButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> ClassicScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMultiLineEditableTextBox> MazeScore;
};
