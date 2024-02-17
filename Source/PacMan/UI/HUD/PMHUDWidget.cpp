// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "UI/HUD/PMHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPMHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Life1->SetVisibility(ESlateVisibility::Visible);
	Life2->SetVisibility(ESlateVisibility::Visible);
}

void UPMHUDWidget::SetValues(int32 InScore, int32 InCherryNumber)
{
	Score->SetText(FText::AsNumber(InScore));
	CherryNumber->SetText(FText::AsNumber(InCherryNumber));
}

void UPMHUDWidget::UpdateScore(int32 NewScore)
{
	Score->SetText(FText::AsNumber(NewScore));
}

void UPMHUDWidget::UpdateCherry(int32 NewCherryNumber)
{
	CherryNumber->SetText(FText::AsNumber(NewCherryNumber));
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
