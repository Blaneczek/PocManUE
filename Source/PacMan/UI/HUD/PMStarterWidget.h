// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMStarterWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class PACMAN_API UPMStarterWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	void Counter();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DisplayTime;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ScreenFadeAnim;

private:
	int32 Number;

	FTimerHandle CounterTimer;
};
