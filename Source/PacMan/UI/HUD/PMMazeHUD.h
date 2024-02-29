// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMHUDWidget.h"
#include "PMMazeHUD.generated.h"

class UImage;
class UWidgetAnimation;
class UMaterial;

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
	void SetMapVisibility(ESlateVisibility InVisibility);
	void SetMapDisplay(int32 LevelNum);
	
	void ShowChaseScreen();
	void HideChaseScreen();

	void ShowVulnerableScreen();
	void HideVulnerableScreen();

	void UpdateMapIcon(int32 MapNumber, ESlateVisibility IconVisibility);
	void ShowMapsText(const FText& Text);

	void ShowFullLivesText();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan");
	TObjectPtr<UMaterial> M_Map01;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan");
	TObjectPtr<UMaterial> M_Map02;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan");
	TObjectPtr<UMaterial> M_Map03; 

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> MapDisplay;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> MapIcon1;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> MapIcon2;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> ChaseScreen;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UImage> VulnerableScreen;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UTextBlock> MapsText;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", meta = (BindWidget))
	TObjectPtr<UTextBlock> FullLivesText;

	UPROPERTY(BlueprintReadOnly, Category = "PocMan", Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ChaseFlickeringAnim;
	UPROPERTY(BlueprintReadOnly, Category = "PocMan", Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VulnerableFlickeringAnim;

private:
	FTimerHandle MapsTextTimer;
	FTimerHandle FullLivesTextTimer;
};
