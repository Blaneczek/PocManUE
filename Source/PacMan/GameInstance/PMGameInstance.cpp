// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMGameInstance.h"
#include "Saved/PMSaveGame.h"
#include "Kismet/GameplayStatics.h"

ELevelType UPMGameInstance::CurrentLevelType = ELevelType::MENU;

void UPMGameInstance::Init()
{
	Super::Init();

	// Names must match the map names in the editor
	ClassicLevels.Add(1, FName(TEXT("Classic01")));
	ClassicLevels.Add(2, FName(TEXT("Classic02")));
	ClassicLevels.Add(3, FName(TEXT("Classic03")));
	MazeLevels.Add(1, FName(TEXT("Maze01")));
	MazeLevels.Add(2, FName(TEXT("Maze02")));
	MazeLevels.Add(3, FName(TEXT("Maze03")));

	SaveSlotName = FString(TEXT("SaveSlot"));

	SetLevelType(ELevelType::MENU);
}

FText UPMGameInstance::FormatScoreboardData(const TArray<FScoreboardData>& FinalScoresData) const
{
	FString FinalScores = FString(TEXT(""));
	for (int32 index = 0; index < FinalScoresData.Num(); ++index)
	{
		FString itemScores = FString::FromInt(index + 1)
			+ ".Score:" + FString::FromInt(FinalScoresData[index].Score)
			+ " | Cherries:" + FString::FromInt(FinalScoresData[index].Cherries)
			+ " | Date:" + FinalScoresData[index].Date
			+ LINE_TERMINATOR TEXT("\n");
		FinalScores.Append(itemScores);
	}

	return FText::FromString(FinalScores);
}

FText UPMGameInstance::GetScoreboardData(ELevelType LevelType) const
{
	// Switch because Text in Scoreboard widget is set individually in Classic and Maze column
	switch (LevelType)
	{
		case ELevelType::CLASSIC: return FormatScoreboardData(ClassicScoreboardData);
		case ELevelType::MAZE: return FormatScoreboardData(MazeScoreboardData);
		case ELevelType::MENU: return FText::FromString("");
	}

	return FText::FromString("");
}

void UPMGameInstance::ClearScoreboardData()
{
	if (ClassicScoreboardData.IsEmpty() && MazeScoreboardData.IsEmpty())
	{
		return;		
	}

	ClassicScoreboardData.Empty();
	MazeScoreboardData.Empty();
	SaveGame();
}


void UPMGameInstance::AddScoreboardData(ELevelType LevelType, const FScoreboardData& ScoreboardData)
{
	switch (LevelType)
	{
		case ELevelType::CLASSIC:
		{
			ClassicScoreboardData.Add(ScoreboardData);
			// Sorts the scores to display from best to worst 
			ClassicScoreboardData.Sort([](const FScoreboardData& a, const FScoreboardData& b) { return a.Score > b.Score; });
			return;
		}
		case ELevelType::MAZE:
		{
			MazeScoreboardData.Add(ScoreboardData);
			MazeScoreboardData.Sort([](const FScoreboardData& a, const FScoreboardData& b) { return a.Score > b.Score; });
			return;
		}
		default: return;
	}	
}

void UPMGameInstance::SaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveGameInstance = Cast<UPMSaveGame>(UGameplayStatics::CreateSaveGameObject(UPMSaveGame::StaticClass()));
		if (SaveGameInstance == nullptr)
		{
			return;
		}				
	}

	// Sets gameplay and scores data in SaveGame object
	SaveGameInstance->SetSaveData(ClassicGameData, MazeGameData, ClassicScoreboardData, MazeScoreboardData);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
}

void UPMGameInstance::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveGameInstance = Cast<UPMSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (SaveGameInstance != nullptr)
		{
			// Takes data from SaveGame object and assigns it to data in the game instance
			SaveGameInstance->GetSaveData(ClassicGameData, MazeGameData, ClassicScoreboardData, MazeScoreboardData);
		}		
	}
}



