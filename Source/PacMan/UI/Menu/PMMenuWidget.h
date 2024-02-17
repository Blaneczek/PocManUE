// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMMenuWidget.generated.h"

class UButton;
class UBorder;
class UPMScoreboardWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewClassic, ELevelType, levelType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FContinueClassic, ELevelType, levelType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewMaze, ELevelType, levelType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FContinueMaze, ELevelType, levelType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitGame);

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

protected:
	//Buttons
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClassicButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MazeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ScoreboardButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitGameButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> XClassicButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> XMazeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NewClassicButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NewMazeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueClassicButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueMazeButton;

	// Classic/Maze starters
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ClassicStarter;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> MazeStarter;

	// Scoreboard
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPMScoreboardWidget> Scoreboard;

public:
	//Delegates
	UPROPERTY()
	FNewClassic OnStartNewClassic;
	UPROPERTY()
	FNewMaze OnStartNewMaze;
	UPROPERTY()
	FContinueClassic OnContinueClassic;
	UPROPERTY()
	FContinueMaze OnContinueMaze;
	UPROPERTY()
	FExitGame OnExitGame;
};
