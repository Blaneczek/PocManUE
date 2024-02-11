// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMStarterWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UPMStarterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Number = 3;

	DisplayTime->SetText(FText::AsNumber(Number));
	PlayAnimation(ScreenFadeAnim);

	GetWorld()->GetTimerManager().SetTimer(CounterTimer, this, &UPMStarterWidget::Counter, 1, true);
}

void UPMStarterWidget::Counter()
{
	DisplayTime->SetText(FText::AsNumber(--Number));

	if (Number == 0 && CounterTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CounterTimer);
		DisplayTime->SetText(FText::FromString("LET'S GO!"));

		FTimerHandle RemoveTimer;
		GetWorld()->GetTimerManager().SetTimer(RemoveTimer, this, &UPMStarterWidget::RemoveFromParent, 1, false);
	}
}

