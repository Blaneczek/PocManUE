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
	int32 CherryNumber;

	FGameData()
	{
		LevelNum = 1;
		Score = 0;
		CherryNumber = 0;
	}

	FGameData(int32 LevelNum, int32 ScoreNum, int32 CherryNum)
		: LevelNum(LevelNum), Score(ScoreNum), CherryNumber(CherryNum)
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

	FScoreboardData(int32 ScoreNum, int32 CherryNum)
		: Score(ScoreNum), Cherries(CherryNum)
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

	TSubclassOf<APMCamera> GetCameraClass() const;

	void SetLevelType(ELevelType LevelType) { CurrentLevelType = LevelType; };
	ELevelType GetCurrentLevelType() const { return CurrentLevelType; }

	void SetClassicData(const FGameData& Data) { ClassicGameData = Data; }

	void SetMazeData(const FGameData& Data) { MazeGameData = Data; }

	FText GetScoreData(ELevelType LevelType) const;
	void ClearScoreData();

	void AddScore(ELevelType LevelType, const FScoreboardData& ScoreData);

	void SaveGame();
	void LoadGame();

private:
	FText MakeScoreDataAsText(const TArray<FScoreboardData>& ScoreData) const;


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
	UPROPERTY(EditDefaultsOnly, Category = "PocMan | Camera")
	TSubclassOf<APMCamera> ClassicCameraClass;

	UPROPERTY()
	ELevelType CurrentLevelType;

	UPROPERTY()
	TArray<FScoreboardData> ClassicScoreData;
	UPROPERTY()
	TArray<FScoreboardData> MazeScoreData;

	FString SaveSlotName;

	UPROPERTY()
	TObjectPtr<UPMSaveGame> SaveGameInstance;
};
