// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMMenuWidget.generated.h"

class UButton;
class UBorder;
class UPMScoreboardWidget;
class USlider;
class USoundClass;
class USoundMix;
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

public:
	void SetIsEnabledClassicButton(bool bInIsEnabled);
	void SetIsEnabledMazeButton(bool bInIsEnabled);	

protected:
	void NativeConstruct() override;

	/** Buttons handle */
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
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnXSettingsButtonClicked();
	
	UFUNCTION()
	void SetAudioMasterValue(float Value);

	void ToggleButtons(bool bInIsEnabled);

private:
	void BindButtons();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> ClassicButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> MazeButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> ScoreboardButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> ExitGameButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> XClassicButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> XMazeButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> NewClassicButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> NewMazeButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> ContinueClassicButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> ContinueMazeButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UButton> XSettingsButton;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UBorder> ClassicStarter;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UBorder> MazeStarter;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UBorder> Settings;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UPMScoreboardWidget> Scoreboard;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<USlider> MasterAudioSlider;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	TObjectPtr<USoundClass> MasterSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	TObjectPtr<USoundMix> MasterSoundMixClass;

public:
	/** Delegates to be bound in MenuGameMode */
	FOnNewClassic OnStartNewClassic;	
	FOnNewMaze OnStartNewMaze;	
	FOnContinueClassic OnContinueClassic;	
	FOnContinueMaze OnContinueMaze;	
	FOnExitGame OnExitGame;
};
