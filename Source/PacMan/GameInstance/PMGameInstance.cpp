// Fill out your copyright notice in the Description page of Project Settings.

#include "PMGameInstance.h"

void UPMGameInstance::Init()
{
	Super::Init();

	SetLevel(ELevelType::MENU);

}

TSubclassOf<APMCamera> UPMGameInstance::GetCameraClass() const
{
	switch (CurrentLevelType)
	{
		case ELevelType::CLASSIC: return ClassicCameraClass;
		case ELevelType::MAZE: return nullptr;
	}
	return nullptr;
}

FText UPMGameInstance::MakeScoreDataAsText(const TArray<FScoreboardData>& ScoreData) const
{
	FString score = "";
	for (int32 index = 0; index < ScoreData.Num(); ++index)
	{
		FString itemScore = FString::FromInt(index + 1)
			+ ". Score: " + FString::FromInt(ScoreData[index].Score)
			+ " | Cherries: " + FString::FromInt(ScoreData[index].Cherries)
			+ " | Date: " + ScoreData[index].Date
			+ LINE_TERMINATOR TEXT("\n");
		score.Append(itemScore);
	}

	return FText::FromString(score);
}


FText UPMGameInstance::GetScoreData(ELevelType LevelType) const
{
	switch (LevelType)
	{
		case ELevelType::CLASSIC: return MakeScoreDataAsText(ClassicScoreData);
		case ELevelType::MAZE: return MakeScoreDataAsText(MazeScoreData);
	}
	return FText::FromString("");
}


void UPMGameInstance::AddScore(ELevelType LevelType, const FScoreboardData& ScoreData)
{
	switch (LevelType)
	{
		case ELevelType::CLASSIC:
		{
			ClassicScoreData.Add(ScoreData);
			ClassicScoreData.Sort([](const FScoreboardData& a, const FScoreboardData& b) { return a.Score > b.Score; });
			return;
		}
		case ELevelType::MAZE:
		{
			MazeScoreData.Add(ScoreData);
			MazeScoreData.Sort([](const FScoreboardData& a, const FScoreboardData& b) { return a.Score > b.Score; });
			return;
		}
		default: return;
	}
	
}



