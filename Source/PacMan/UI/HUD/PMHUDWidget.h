// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMHUDWidget.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class PACMAN_API UPMHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateScore(int32 newScore);
	void UpdateCherry(int32 newCherryNumber);
	void UpdateLife(int32 Lives);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Score;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CherryNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Life1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Life2;
};
