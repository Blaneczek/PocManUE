// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMMenuWidget.generated.h"

class UButton;
class UBorder;
class UPMScoreboardWidget;
enum class ELevelType : uint8;

DECLARE_DELEGATE_OneParam(FOnNewClassic, ELevelType);
DECLARE_DELEGATE_OneParam(FOnContinueClassic, ELevelType);
DECLARE_DELEGATE_OneParam(FOnNewMaze, ELevelType);
DECLARE_DELEGATE_OneParam(FOnContinueMaze, ELevelType);
DECLARE_DELEGATE(FOnExitGame);

/**
 * 
 */
UCLASS()
class PACMAN_API UPMMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void NativeConstruct() override;

	//Buttons handle
	UFUNCTION()
	void OnClassicButtonClicked();
	UFUNCTION()
	void OnMazeButtonClicked();
	UFUNCTION()
	void OnScoreboardButtonClicked();
	UFUNCTION()
	void OnExitGameButtonClicked();
	UFUNCTION()
	void OnXClassicButtonClicked();
	UFUNCTION()
	void OnXMazeButtonClicked();
	UFUNCTION()
	void OnNewClassicButtonClicked();
	UFUNCTION()
	void OnNewMazeButtonClicked();
	UFUNCTION()
	void OnContinueClassicButtonClicked();
	UFUNCTION()
	void OnContinueMazeButtonClicked();

	void ToggleButtons(bool bInIsEnabled);

public:
	void SetIsEnabledClassicButton(bool bInIsEnabled);
	void SetIsEnabledMazeButton(bool bInIsEnabled);

private:
	void BindButtons();

protected:
	//Buttons
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ClassicButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> MazeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ScoreboardButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitGameButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> XClassicButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> XMazeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> NewClassicButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> NewMazeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ContinueClassicButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ContinueMazeButton;

	// Classic/Maze starters
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UBorder> ClassicStarter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UBorder> MazeStarter;

	// Scoreboard
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPMScoreboardWidget> Scoreboard;

public:
	// Delegates	
	FOnNewClassic OnStartNewClassic;	
	FOnNewMaze OnStartNewMaze;	
	FOnContinueClassic OnContinueClassic;	
	FOnContinueMaze OnContinueMaze;	
	FOnExitGame OnExitGame;
};
