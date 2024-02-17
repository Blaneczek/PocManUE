// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMSaveGame.h"

void UPMSaveGame::SetSaveData(const FGameData& InClassicGameData, const FGameData& InMazeGameData, const TArray<FScoreboardData>& InClassicScoreData, const TArray<FScoreboardData>& InMazeScoreData)
{
	ClassicGameData = InClassicGameData;
	MazeGameData = InMazeGameData;
	ClassicScoreData = InClassicScoreData;
	MazeScoreData = InMazeScoreData;
}

void UPMSaveGame::GetSaveData(FGameData& OutClassicGameData, FGameData& OutMazeGameData, TArray<FScoreboardData>& OutClassicScoreData, TArray<FScoreboardData>& OutMazeScoreData)
{
	OutClassicGameData = ClassicGameData;
	OutMazeGameData = MazeGameData;
	OutClassicScoreData = ClassicScoreData;
	OutMazeScoreData = MazeScoreData;
}
