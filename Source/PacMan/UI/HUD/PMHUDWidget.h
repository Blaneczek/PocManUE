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
	void SetScore(int32 InScore);
	void SetCherries(int32 InCherries);
	void UpdateLives(int32 LifeNum, ESlateVisibility IconVisibility);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UTextBlock> Score;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UTextBlock> Cherries;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> Life1;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> Life2;
};
