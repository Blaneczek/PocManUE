// Fill out your copyright notice in the Description page of Project Settings.

#include "PMGameInstance.h"
#include "Saved/PMSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UPMGameInstance::Init()
{
	Super::Init();

	ClassicLevels.Add(1, "Classic01");
	ClassicLevels.Add(2, "Classic02");

	MazeLevels.Add(1, "Maze01");
	MazeLevels.Add(2, "Maze02");

	SaveSlotName = "SaveSlot";
	SetLevelType(ELevelType::MENU);

	LoadGame();
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

void UPMGameInstance::SaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveGameInstance->SetSaveData(ClassicGameData, MazeGameData, ClassicScoreData, MazeScoreData);
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
	}
	else
	{
		SaveGameInstance = Cast<UPMSaveGame>(UGameplayStatics::CreateSaveGameObject(UPMSaveGame::StaticClass()));
		SaveGameInstance->SetSaveData(ClassicGameData, MazeGameData, ClassicScoreData, MazeScoreData);
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
	}
}

void UPMGameInstance::LoadGame()
{
	UE_LOG(LogTemp, Warning, TEXT("load game"));
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("load game exist"));
		SaveGameInstance = Cast<UPMSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		SaveGameInstance->GetSaveData(ClassicGameData, MazeGameData, ClassicScoreData, MazeScoreData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("load game doesnt exist"));
	}
}



