// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMGameInstance.h"
#include "Saved/PMSaveGame.h"
#include "Kismet/GameplayStatics.h"

ELevelType UPMGameInstance::CurrentLevelType = ELevelType::MENU;

void UPMGameInstance::Init()
{
	Super::Init();

	ClassicLevels.Add(1, "Classic01");
	ClassicLevels.Add(2, "Classic02");
	ClassicLevels.Add(3, "Classic03");

	MazeLevels.Add(1, "Maze01");
	MazeLevels.Add(2, "Maze02");
	MazeLevels.Add(3, "Maze03");

	SaveSlotName = "SaveSlot";

	SetLevelType(ELevelType::MENU);

	LoadGame();
}

FText UPMGameInstance::MakeScoreboardDataAsText(const TArray<FScoreboardData>& FinalScoresData) const
{
	FString FinalScores = "";
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
	switch (LevelType)
	{
		case ELevelType::CLASSIC: return MakeScoreboardDataAsText(ClassicScoreboardData);
		case ELevelType::MAZE: return MakeScoreboardDataAsText(MazeScoreboardData);
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


void UPMGameInstance::AddScoreboardData(ELevelType LevelType, const FScoreboardData& ScoreData)
{
	switch (LevelType)
	{
		case ELevelType::CLASSIC:
		{
			ClassicScoreboardData.Add(ScoreData);
			ClassicScoreboardData.Sort([](const FScoreboardData& a, const FScoreboardData& b) { return a.Score > b.Score; });
			return;
		}
		case ELevelType::MAZE:
		{
			MazeScoreboardData.Add(ScoreData);
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
			SaveGameInstance->GetSaveData(ClassicGameData, MazeGameData, ClassicScoreboardData, MazeScoreboardData);
		}		
	}
}



