// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameInstance/PMGameInstance.h"
#include "PMSaveGame.generated.h"


/**
 * 
 */
UCLASS()
class PACMAN_API UPMSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	void SetSaveData(const FGameData& InClassicGameData, const FGameData& InMazeGameData, const TArray<FScoreboardData>& InClassicScoreData, const TArray<FScoreboardData>& InMazeScoreData);
	void GetSaveData(FGameData& OutClassicGameData, FGameData& OutMazeGameData, TArray<FScoreboardData>& OutClassicScoreData, TArray<FScoreboardData>& OutMazeScoreData);

public:
	UPROPERTY()
	FGameData ClassicGameData;
	UPROPERTY()
	FGameData MazeGameData;
	UPROPERTY()
	TArray<FScoreboardData> ClassicScoreData;
	UPROPERTY()
	TArray<FScoreboardData> MazeScoreData;
};
