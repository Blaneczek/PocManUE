// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMHUDWidget.h"
#include "PMMazeHUD.generated.h"

class UImage;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class PACMAN_API UPMMazeHUD : public UPMHUDWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:
	void ShowMap();
	void HideMap();
	
	void ShowChaseScreen();
	void HideChaseScreen();

	void ShowVulnerableScreen();
	void HideVulnerableScreen();

	void UpdateMapIcon(int32 MapNumber, ESlateVisibility IconVisibility);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapDisplay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapIcon1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapIcon2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ChaseScreen;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> VulnerableScreen;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ChaseFlickeringAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VulnerableFlickeringAnim;
};
