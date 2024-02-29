// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMenuWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "GameInstance/PMGameInstance.h"
#include "PMScoreboardWidget.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

void UPMMenuWidget::NativeConstruct()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Hidden);
	MazeStarter->SetVisibility(ESlateVisibility::Hidden);
	Settings->SetVisibility(ESlateVisibility::Hidden);

	BindButtons();

	if (MasterSoundClass != nullptr && MasterSoundMixClass != nullptr)
	{
		MasterAudioSlider->OnValueChanged.AddDynamic(this, &UPMMenuWidget::SetAudioMasterValue);
	}
}

void UPMMenuWidget::OnClassicButtonClicked()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Visible);
	ToggleButtons(false);
}

void UPMMenuWidget::OnMazeButtonClicked()
{
	MazeStarter->SetVisibility(ESlateVisibility::Visible);
	ToggleButtons(false);
}

void UPMMenuWidget::OnScoreboardButtonClicked()
{
	Scoreboard->SetFinalScores();
	Scoreboard->SetVisibility(ESlateVisibility::Visible);
}

void UPMMenuWidget::OnExitGameButtonClicked()
{
	OnExitGame.ExecuteIfBound();
}

void UPMMenuWidget::OnXClassicButtonClicked()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Hidden);
	ToggleButtons(true);
}

void UPMMenuWidget::OnXMazeButtonClicked()
{
	MazeStarter->SetVisibility(ESlateVisibility::Hidden);
	ToggleButtons(true);
}

void UPMMenuWidget::OnNewClassicButtonClicked()
{
	OnStartNewClassic.ExecuteIfBound(ELevelType::CLASSIC);
}

void UPMMenuWidget::OnNewMazeButtonClicked()
{
	OnStartNewMaze.ExecuteIfBound(ELevelType::MAZE);
}

void UPMMenuWidget::OnContinueClassicButtonClicked()
{
	OnContinueClassic.ExecuteIfBound(ELevelType::CLASSIC);
}

void UPMMenuWidget::OnContinueMazeButtonClicked()
{
	OnContinueMaze.ExecuteIfBound(ELevelType::MAZE);
}

void UPMMenuWidget::OnSettingsButtonClicked()
{
	Settings->SetVisibility(ESlateVisibility::Visible);
	ToggleButtons(false);
}

void UPMMenuWidget::OnXSettingsButtonClicked()
{
	Settings->SetVisibility(ESlateVisibility::Hidden);
	ToggleButtons(true);
}

void UPMMenuWidget::SetIsEnabledClassicButton(bool bInIsEnabled)
{
	ContinueClassicButton->SetIsEnabled(bInIsEnabled);
}

void UPMMenuWidget::SetIsEnabledMazeButton(bool bInIsEnabled)
{
	ContinueMazeButton->SetIsEnabled(bInIsEnabled);
}

void UPMMenuWidget::BindButtons()
{
	ClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnClassicButtonClicked);
	MazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnMazeButtonClicked);
	ScoreboardButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnScoreboardButtonClicked);
	ExitGameButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnExitGameButtonClicked);
	XClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnXClassicButtonClicked);
	XMazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnXMazeButtonClicked);
	NewClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnNewClassicButtonClicked);
	NewMazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnNewMazeButtonClicked);
	ContinueClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnContinueClassicButtonClicked);
	ContinueMazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnContinueMazeButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnSettingsButtonClicked);
	XSettingsButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnXSettingsButtonClicked);
}

void UPMMenuWidget::ToggleButtons(bool bInIsEnabled)
{
	ClassicButton->SetIsEnabled(bInIsEnabled);
	MazeButton->SetIsEnabled(bInIsEnabled);
	ScoreboardButton->SetIsEnabled(bInIsEnabled);
	ExitGameButton->SetIsEnabled(bInIsEnabled);
	SettingsButton->SetIsEnabled(bInIsEnabled);
}

void UPMMenuWidget::SetAudioMasterValue(float Value)
{
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMixClass, MasterSoundClass, Value);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMixClass);
}
