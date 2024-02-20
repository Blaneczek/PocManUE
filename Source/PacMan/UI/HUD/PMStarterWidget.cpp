// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMStarterWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UPMStarterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NumberToShow = 3;

	if (ScreenFadeAnim != nullptr)
	{
		PlayAnimation(ScreenFadeAnim);
	}
	
	DisplayTime->SetText(FText::AsNumber(NumberToShow));
	GetWorld()->GetTimerManager().SetTimer(TextCounterTimer, this, &UPMStarterWidget::TextCounter, 1, true);
}

void UPMStarterWidget::TextCounter()
{
	DisplayTime->SetText(FText::AsNumber(--NumberToShow));

	if (NumberToShow == 0 && TextCounterTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TextCounterTimer);
		DisplayTime->SetText(FText::FromString("LET'S GO!"));

		FTimerHandle RemoveWidgetTimer;
		GetWorld()->GetTimerManager().SetTimer(RemoveWidgetTimer, this, &UPMStarterWidget::RemoveFromParent, 1, false);
	}
}

