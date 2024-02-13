// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUD/PMHUDWidget.h"
#include "PMMazeHUD.generated.h"

class UImage;
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

	void UpdateMapIcon(int32 Maps);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapDisplay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapIcon1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapIcon2;
};
