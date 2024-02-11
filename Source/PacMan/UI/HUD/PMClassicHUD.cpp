// Fill out your copyright notice in the Description page of Project Settings.


#include "PMClassicHUD.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPMClassicHUD::NativeConstruct()
{
	Super::NativeConstruct();

	Score->SetText(FText::AsNumber(0));
	CherryNumber->SetText(FText::AsNumber(0));
	Life1->SetVisibility(ESlateVisibility::Visible);
	Life2->SetVisibility(ESlateVisibility::Visible);
}

void UPMClassicHUD::UpdateScore(int32 newScore)
{
	Score->SetText(FText::AsNumber(newScore));
}

void UPMClassicHUD::UpdateCherry(int32 newCherryNumber)
{
	CherryNumber->SetText(FText::AsNumber(newCherryNumber));
}

void UPMClassicHUD::UpdateLife(int32 Lives)
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
