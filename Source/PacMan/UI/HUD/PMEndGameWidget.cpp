// Fill out your copyright notice in the Description page of Project Settings.


#include "PMEndGameWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UPMEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton->OnClicked.AddDynamic(this, &UPMEndGameWidget::OnMenuButtonClicked);
	RestartButton->OnClicked.AddDynamic(this, &UPMEndGameWidget::OnRestartButtonClicked);
}

void UPMEndGameWidget::OnMenuButtonClicked()
{
	OnBackToMenu.Broadcast();
}

void UPMEndGameWidget::OnRestartButtonClicked()
{
	OnRestartGame.Broadcast();
}

void UPMEndGameWidget::SetScore(int32 Score, int32 Cherries)
{
	ScoreText->SetText(FText::FromString("Score: " + FString::FromInt(Score) + " | Cherries: " + FString::FromInt(Cherries)));
}
