// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "UI/HUD/PMHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPMHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Score->SetText(FText::AsNumber(0));
	CherryNumber->SetText(FText::AsNumber(0));
	Life1->SetVisibility(ESlateVisibility::Visible);
	Life2->SetVisibility(ESlateVisibility::Visible);
}

void UPMHUDWidget::UpdateScore(int32 newScore)
{
	Score->SetText(FText::AsNumber(newScore));
}

void UPMHUDWidget::UpdateCherry(int32 newCherryNumber)
{
	CherryNumber->SetText(FText::AsNumber(newCherryNumber));
}

void UPMHUDWidget::UpdateLife(int32 Lives)
{
	if (Lives == 2)
	{
		Life2->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (Lives == 1)
	{
		Life1->SetVisibility(ESlateVisibility::Hidden);
	}
}
