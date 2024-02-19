// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazeHUD.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

void UPMMazeHUD::NativeConstruct()
{
	Super::NativeConstruct();

	Life1->SetVisibility(ESlateVisibility::Visible);
	Life2->SetVisibility(ESlateVisibility::Visible);
	MapDisplay->SetVisibility(ESlateVisibility::Visible);
	ChaseScreen->SetVisibility(ESlateVisibility::Hidden);
	VulnerableScreen->SetVisibility(ESlateVisibility::Hidden);
	
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

void UPMMazeHUD::SetMap(int32 LevelNum)
{
	switch (LevelNum)
	{
		case 1:
		{
			UE_LOG(LogTemp, Warning, TEXT("map01"));
			MapDisplay->SetBrushFromMaterial(M_Map01);
			MapDisplay->SetBrushSize(FVector2D(700.f, 340.f));
			return;
		}
		case 2:
		{
			UE_LOG(LogTemp, Warning, TEXT("map02"));
			MapDisplay->SetBrushFromMaterial(M_Map02);
			MapDisplay->SetBrushSize(FVector2D(450.f, 420.f));
			return;
		}
		case 3:
		{

		}
	}
}

void UPMMazeHUD::ShowChaseScreen()
{
	ChaseScreen->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(ChaseFlickeringAnim, 0.f, 0);
}

void UPMMazeHUD::HideChaseScreen()
{
	ChaseScreen->SetVisibility(ESlateVisibility::Hidden);
	StopAnimation(ChaseFlickeringAnim);
}

void UPMMazeHUD::ShowVulnerableScreen()
{
	VulnerableScreen->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(VulnerableFlickeringAnim, 0.f, 0);
}

void UPMMazeHUD::HideVulnerableScreen()
{
	VulnerableScreen->SetVisibility(ESlateVisibility::Hidden);
	StopAnimation(VulnerableFlickeringAnim);
}

void UPMMazeHUD::UpdateMapIcon(int32 MapNumber, ESlateVisibility IconVisibility)
{
	if (MapNumber == 2)
	{
		MapIcon2->SetVisibility(IconVisibility);
	}
	else if (MapNumber == 1)
	{
		MapIcon1->SetVisibility(IconVisibility);
	}
}
