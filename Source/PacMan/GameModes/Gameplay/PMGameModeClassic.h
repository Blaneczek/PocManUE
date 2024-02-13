// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMGameModeBase.h"
#include "PMGameModeClassic.generated.h"


class UPMClassicHUD;

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeClassic : public APMGameModeBase
{
	GENERATED_BODY()

public:
	//Default constructor
	APMGameModeClassic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeWidgets(APlayerController* PlayerController) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMClassicHUD> ClassicHUDClass;

};
