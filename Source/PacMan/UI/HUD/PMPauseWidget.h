// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PMPauseWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackToMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContinue);

/**
 * 
 */
UCLASS()
class PACMAN_API UPMPauseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMenuButtonClicked();
	UFUNCTION()
	void OnContinueButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

public:
	UPROPERTY()
	FOnBackToMenu OnBackToMenu;
	UPROPERTY()
	FOnContinue OnContinue;
};
