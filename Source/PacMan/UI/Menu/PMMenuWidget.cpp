// Fill out your copyright notice in the Description page of Project Settings.


#include "PMMenuWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "GameInstance/PMGameInstance.h"
#include "PMScoreboardWidget.h"

void UPMMenuWidget::NativeConstruct()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Hidden);
	MazeStarter->SetVisibility(ESlateVisibility::Hidden);

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
}

void UPMMenuWidget::OnClassicButtonClicked()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Visible);
}

void UPMMenuWidget::OnMazeButtonClicked()
{
	MazeStarter->SetVisibility(ESlateVisibility::Visible);
}

void UPMMenuWidget::OnScoreboardButtonClicked()
{
	Scoreboard->SetScores();
	Scoreboard->SetVisibility(ESlateVisibility::Visible);
}

void UPMMenuWidget::OnExitGameButtonClicked()
{
	OnExitGame.Broadcast();
}

void UPMMenuWidget::OnXClassicButtonClicked()
{
	ClassicStarter->SetVisibility(ESlateVisibility::Hidden);
}

void UPMMenuWidget::OnXMazeButtonClicked()
{
	MazeStarter->SetVisibility(ESlateVisibility::Hidden);
}

void UPMMenuWidget::OnNewClassicButtonClicked()
{
	OnStartNewClassic.Broadcast(ELevelType::CLASSIC);
}

void UPMMenuWidget::OnNewMazeButtonClicked()
{
	OnStartNewMaze.Broadcast(ELevelType::MAZE);
}

void UPMMenuWidget::OnContinueClassicButtonClicked()
{
	OnContinueClassic.Broadcast(ELevelType::CLASSIC);
}

void UPMMenuWidget::OnContinueMazeButtonClicked()
{
	OnContinueMaze.Broadcast(ELevelType::MAZE);
}
