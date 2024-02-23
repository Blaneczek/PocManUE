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

void UPMHUDWidget::SetScore(int32 InScore)
{
	Score->SetText(FText::AsNumber(InScore));
}

void UPMHUDWidget::SetCherries(int32 InCherries)
{
	Cherries->SetText(FText::AsNumber(InCherries));
}

void UPMHUDWidget::UpdateLives(int32 LifeNum, ESlateVisibility IconVisibility)
{
	if (LifeNum == 2)
	{
		Life2->SetVisibility(IconVisibility);
	}
	else if (LifeNum == 1)
	{
		Life1->SetVisibility(IconVisibility);
	}
}
