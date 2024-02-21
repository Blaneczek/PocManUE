// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMSaveGame.h"
#include "GameInstance/PMGameInstance.h"

void UPMSaveGame::SetSaveData(const FGameData& InClassicGameData, const FGameData& InMazeGameData, const TArray<FScoreboardData>& InClassicScoreboardData, const TArray<FScoreboardData>& InMazeScoreboardData)
{
	ClassicGameData = InClassicGameData;
	MazeGameData = InMazeGameData;
	ClassicScoreboardData = InClassicScoreboardData;
	MazeScoreboardData = InMazeScoreboardData;
}

void UPMSaveGame::GetSaveData(FGameData& OutClassicGameData, FGameData& OutMazeGameData, TArray<FScoreboardData>& OutClassicScoreboardData, TArray<FScoreboardData>& OutMazeScoreboardData)
{
	OutClassicGameData = ClassicGameData;
	OutMazeGameData = MazeGameData;
	OutClassicScoreboardData = ClassicScoreboardData;
	OutMazeScoreboardData = MazeScoreboardData;
}
