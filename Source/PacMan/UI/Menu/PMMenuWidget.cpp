// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMenuWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "GameInstance/PMGameInstance.h"
#include "PMScoreboardWidget.h"

void UPMMenuWidget::NativeConstruct()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Hidden);
	MazeStarter->SetVisibility(ESlateVisibility::Hidden);

	BindButtons();
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
	Scoreboard->SetScores();
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
	if (ClassicButton != nullptr)
		ClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnClassicButtonClicked);

	if (MazeButton != nullptr)
		MazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnMazeButtonClicked);

	if (ScoreboardButton != nullptr)
		ScoreboardButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnScoreboardButtonClicked);

	if (ExitGameButton != nullptr)
		ExitGameButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnExitGameButtonClicked);

	if (XClassicButton != nullptr)
		XClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnXClassicButtonClicked);

	if (XMazeButton != nullptr)
		XMazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnXMazeButtonClicked);

	if (NewClassicButton != nullptr)
		NewClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnNewClassicButtonClicked);

	if (NewMazeButton != nullptr)
		NewMazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnNewMazeButtonClicked);

	if (ContinueClassicButton != nullptr)
		ContinueClassicButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnContinueClassicButtonClicked);

	if (ContinueMazeButton != nullptr)
		ContinueMazeButton->OnClicked.AddDynamic(this, &UPMMenuWidget::OnContinueMazeButtonClicked);
}

void UPMMenuWidget::ToggleButtons(bool bInIsEnabled)
{
	ClassicButton->SetIsEnabled(bInIsEnabled);
	MazeButton->SetIsEnabled(bInIsEnabled);
	ScoreboardButton->SetIsEnabled(bInIsEnabled);
	ExitGameButton->SetIsEnabled(bInIsEnabled);
}
