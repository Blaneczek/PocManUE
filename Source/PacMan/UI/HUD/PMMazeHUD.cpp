// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazeHUD.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UPMMazeHUD::NativeConstruct()
{
	Super::NativeConstruct();

	Life1->SetVisibility(ESlateVisibility::Visible);
	Life2->SetVisibility(ESlateVisibility::Visible);
	MapDisplay->SetVisibility(ESlateVisibility::Visible);
	ChaseScreen->SetVisibility(ESlateVisibility::Hidden);
	VulnerableScreen->SetVisibility(ESlateVisibility::Hidden);
	MapsText->SetVisibility(ESlateVisibility::Hidden);
	FullLivesText->SetVisibility(ESlateVisibility::Hidden);
	
	FTimerHandle MapTimer;
	FTimerDelegate MapTimerDel;
	MapTimerDel.BindUObject(this, &UPMMazeHUD::SetMapVisibility, ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().SetTimer(MapTimer, MapTimerDel, 3.f, false);
}

void UPMMazeHUD::SetMapVisibility(ESlateVisibility InVisibility)
{
	MapDisplay->SetVisibility(InVisibility);
}

void UPMMazeHUD::SetMapDisplay(int32 LevelNum)
{
	switch (LevelNum)
	{
		case 1:
		{
			MapDisplay->SetBrushFromMaterial(M_Map01);
			MapDisplay->SetDesiredSizeOverride(FVector2D(700.f, 340.f));
			return;
		}
		case 2:
		{
			MapDisplay->SetBrushFromMaterial(M_Map02);
			MapDisplay->SetDesiredSizeOverride(FVector2D(450.f, 420.f));
			return;
		}
		case 3:
		{
			MapDisplay->SetBrushFromMaterial(M_Map03);
			MapDisplay->SetDesiredSizeOverride(FVector2D(450.f, 420.f));
			return;
		}
	}
}

void UPMMazeHUD::ShowChaseScreen()
{
	if (ChaseScreen->GetVisibility() == ESlateVisibility::Hidden)
	{
		ChaseScreen->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(ChaseFlickeringAnim, 0.f, 0);
	}	
}

void UPMMazeHUD::HideChaseScreen()
{
	if (ChaseScreen->GetVisibility() == ESlateVisibility::Visible)
	{
		ChaseScreen->SetVisibility(ESlateVisibility::Hidden);
		StopAnimation(ChaseFlickeringAnim);
	}	
}

void UPMMazeHUD::ShowVulnerableScreen()
{
	if (VulnerableScreen->GetVisibility() == ESlateVisibility::Hidden)
	{
		VulnerableScreen->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(VulnerableFlickeringAnim, 0.f, 0);
	}	
}

void UPMMazeHUD::HideVulnerableScreen()
{
	if (VulnerableScreen->GetVisibility() == ESlateVisibility::Visible)
	{
		VulnerableScreen->SetVisibility(ESlateVisibility::Hidden);
		StopAnimation(VulnerableFlickeringAnim);
	}
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

void UPMMazeHUD::ShowMapsText(const FText& Text)
{
	if (MapsText->GetVisibility() == ESlateVisibility::Visible) return;

	MapsText->SetText(Text);
	MapsText->SetVisibility(ESlateVisibility::Visible);

	if (MapsTextTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MapsTextTimer);
	}

	FTimerDelegate FullMapsDel;
	FullMapsDel.BindLambda([&]() { MapsText->SetVisibility(ESlateVisibility::Hidden); });
	GetWorld()->GetTimerManager().SetTimer(MapsTextTimer, FullMapsDel, 2.f, false);
}

void UPMMazeHUD::ShowFullLivesText()
{
	if (FullLivesText->GetVisibility() == ESlateVisibility::Visible) return;

	FullLivesText->SetVisibility(ESlateVisibility::Visible);

	if (FullLivesTextTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FullLivesTextTimer);
	}

	FTimerDelegate FullLivesDel;
	FullLivesDel.BindLambda([&]() { FullLivesText->SetVisibility(ESlateVisibility::Hidden); });
	GetWorld()->GetTimerManager().SetTimer(FullLivesTextTimer, FullLivesDel, 2.f, false);
}

