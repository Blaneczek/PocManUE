// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMClassicHUD.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class PACMAN_API UPMClassicHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void NativeConstruct() override;

public:
	void UpdateScore(int32 newScore);
	void UpdateCherry(int32 newCherryNumber);
	void UpdateLife(int32 Lives);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CherryNumber;

	UPROPERTY(meta = (BindWidget))
	UImage* Life1;

	UPROPERTY(meta = (BindWidget))
	UImage* Life2;
};

