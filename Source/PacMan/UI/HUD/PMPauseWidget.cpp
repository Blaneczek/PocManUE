// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMPauseWidget.h"
#include "Components/Button.h"

void UPMPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton->OnClicked.AddDynamic(this, &UPMPauseWidget::OnMenuButtonClicked);
	ContinueButton->OnClicked.AddDynamic(this, &UPMPauseWidget::OnContinueButtonClicked);
}

void UPMPauseWidget::OnMenuButtonClicked()
{
	OnBackToMenu.ExecuteIfBound();
}

void UPMPauseWidget::OnContinueButtonClicked()
{
	OnContinue.ExecuteIfBound();
}
