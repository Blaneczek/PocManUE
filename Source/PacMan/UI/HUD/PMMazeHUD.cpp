// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazeHUD.h"
#include "Components/Image.h"

void UPMMazeHUD::NativeConstruct()
{
	Super::NativeConstruct();

	Life1->SetVisibility(ESlateVisibility::Visible);
	Life2->SetVisibility(ESlateVisibility::Visible);
	MapDisplay->SetVisibility(ESlateVisibility::Visible);
	
	FTimerHandle MapTimer;
	GetWorld()->GetTimerManager().SetTimer(MapTimer, this, &UPMMazeHUD::HideMap, 3.f, false);
}

void UPMMazeHUD::ShowMap()
{
	MapDisplay->SetVisibility(ESlateVisibility::Visible);
}

void UPMMazeHUD::HideMap()
{
	MapDisplay->SetVisibility(ESlateVisibility::Hidden);
}

void UPMMazeHUD::UpdateMapIcon(int32 Maps)
{
	if (Maps == 2)
	{
		MapIcon2->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (Maps == 1)
	{
		MapIcon1->SetVisibility(ESlateVisibility::Hidden);
	}
}
