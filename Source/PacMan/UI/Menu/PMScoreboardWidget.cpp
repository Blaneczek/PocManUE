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
}

void UPMScoreboardWidget::OnXButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UPMScoreboardWidget::SetScores()
{
	UPMGameInstance* gameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance != nullptr)
	{
		ClassicScore->SetText(gameInstance->GetScoreData(ELevelType::CLASSIC));
		MazeScore->SetText(gameInstance->GetScoreData(ELevelType::MAZE));
	}
	
}
