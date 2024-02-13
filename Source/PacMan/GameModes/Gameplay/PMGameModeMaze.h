// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "PMGameModeMaze.generated.h"

class UPMMazeHUD;

/**
 * 
 */
UCLASS()
class PACMAN_API APMGameModeMaze : public APMGameModeBase
{
	GENERATED_BODY()

public:
	//Default constructor
	APMGameModeMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeWidgets(APlayerController* PlayerController) override;

public:
	void HideMap();
	void UpdateMaps();
private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Widgets")
	TSubclassOf<UPMMazeHUD> MazeHUDClass;

	int32 MapsNumber;
	bool bIsMapOpen;
};
