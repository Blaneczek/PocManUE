// Copyright (c) 2024 Dawid Szoldra. All rights reserved..


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
	OnBackToMenu.ExecuteIfBound();
}

void UPMEndGameWidget::OnRestartButtonClicked()
{
	OnRestartGame.ExecuteIfBound();
}

void UPMEndGameWidget::SetFinalScores(int32 InScore, int32 InCherries)
{
	FinalScoresText->SetText(FText::FromString("Score:" + FString::FromInt(InScore) + " | Cherries:" + FString::FromInt(InCherries)));
}
