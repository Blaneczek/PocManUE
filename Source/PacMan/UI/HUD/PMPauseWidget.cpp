// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMPauseWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPMPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MenuButton->OnClicked.AddDynamic(this, &UPMPauseWidget::OnMenuButtonClicked);
	ContinueButton->OnClicked.AddDynamic(this, &UPMPauseWidget::OnContinueButtonClicked);
}

void UPMPauseWidget::OnMenuButtonClicked()
{
	OnBackToMenu.Broadcast();
}

void UPMPauseWidget::OnContinueButtonClicked()
{
	OnContinue.Broadcast();
}
