// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMScoreboardWidget.h"
#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"
#include "GameInstance/PMGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UPMScoreboardWidget::NativeConstruct()
{
	SetVisibility(ESlateVisibility::Hidden);

	XScoreButton->OnClicked.AddDynamic(this, &UPMScoreboardWidget::OnXButtonClicked);
	ClearScoreButton->OnClicked.AddDynamic(this, &UPMScoreboardWidget::OnClearButtonClicked);
}

void UPMScoreboardWidget::OnXButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UPMScoreboardWidget::OnClearButtonClicked()
{
	if (UPMGameInstance* GameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GameInstance->ClearScoreData();
		ClassicScore->SetText(FText::FromString(""));
		MazeScore->SetText(FText::FromString(""));
	}
}

void UPMScoreboardWidget::SetScores()
{
	if (UPMGameInstance* GameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		ClassicScore->SetText(GameInstance->GetScoreData(ELevelType::CLASSIC));
		MazeScore->SetText(GameInstance->GetScoreData(ELevelType::MAZE));
	}	
}
