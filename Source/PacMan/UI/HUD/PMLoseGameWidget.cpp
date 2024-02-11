// Fill out your copyright notice in the Description page of Project Settings.


#include "PMLoseGameWidget.h"
#include "Components/Button.h"

void UPMLoseGameWidget::NativeConstruct()
{
	MenuButton->OnClicked.AddDynamic(this, &UPMLoseGameWidget::OnMenuButtonClicked);
	RestartButton->OnClicked.AddDynamic(this, &UPMLoseGameWidget::OnRestartButtonClicked);
}

void UPMLoseGameWidget::OnMenuButtonClicked()
{
	OnBackToMenu.Broadcast();
}

void UPMLoseGameWidget::OnRestartButtonClicked()
{
	OnRestartGame.Broadcast();
}
