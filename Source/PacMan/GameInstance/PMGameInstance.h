// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PMGameInstance.generated.h"

class APMCamera;
class UPMSaveGame;

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	MENU	UMETA(DisplayName = "Menu"),
	CLASSIC	UMETA(DisplayName = "Classic"),
	MAZE	UMETA(DisplayName = "Maze"),
};


USTRUCT()
struct FGameData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 LevelNum;
	UPROPERTY()
	int32 Score;
	UPROPERTY()
	int32 Cherries;

	FGameData()
	{
		LevelNum = 1;
		Score = 0;
		Cherries = 0;
	}

	FGameData(int32 InLevelNum, int32 InScore, int32 InCherries)
		: LevelNum(InLevelNum), Score(InScore), Cherries(InCherries)
	{}
};

USTRUCT()
struct FScoreboardData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Score;
	UPROPERTY()
	int32 Cherries;
	UPROPERTY()
	FString Date;

	FScoreboardData()
	{
		Score = 0;
		Cherries = 0;
		Date = FDateTime::Now().ToString();
	}

	FScoreboardData(int32 InScore, int32 InCherries)
		: Score(InScore), Cherries(InCherries)
	{
		Date = FDateTime::Now().ToString();
	}

	FORCEINLINE bool operator<(const FScoreboardData& ScoreboardData) const
	{
		return (Score < ScoreboardData.Score);
	}
};

/**
 * 
 */
UCLASS(BlueprintType)
class PACMAN_API UPMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void SetLevelType(ELevelType LevelType) { CurrentLevelType = LevelType; };
	FORCEINLINE static ELevelType GetCurrentLevelType() { return CurrentLevelType; }

	void SetClassicData(const FGameData& Data) { ClassicGameData = Data; }
	void SetMazeData(const FGameData& Data) { MazeGameData = Data; }

	FText GetScoreboardData(ELevelType LevelType) const;
	void ClearScoreboardData();
	void AddScoreboardData(ELevelType LevelType, const FScoreboardData& ScoreboardData);

	void SaveGame();
	void LoadGame();

private:
	FText MakeScoreboardDataAsText(const TArray<FScoreboardData>& FinalScoresData) const;

public:
	UPROPERTY()
	FGameData ClassicGameData;
	UPROPERTY()
	FGameData MazeGameData;

	UPROPERTY()
	TMap<int32, FName> ClassicLevels;
	UPROPERTY()
	TMap<int32, FName> MazeLevels;

private:
	static ELevelType CurrentLevelType;

	// Save system
	FString SaveSlotName;
	UPROPERTY()
	TObjectPtr<UPMSaveGame> SaveGameInstance;

	UPROPERTY()
	TArray<FScoreboardData> ClassicScoreboardData;
	UPROPERTY()
	TArray<FScoreboardData> MazeScoreboardData;
};
