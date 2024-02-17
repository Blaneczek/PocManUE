// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PMGameInstance.generated.h"

class APMCamera;

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	MENU	UMETA(DisplayName = "Menu"),
	CLASSIC	UMETA(DisplayName = "Classic"),
	MAZE	UMETA(DisplayName = "Maze"),
};


USTRUCT()
struct FClassicData
{
	GENERATED_BODY()

	FName SubLevel;
	int32 Score;
	int32 Lives;
	int32 CherryNumber;

	FClassicData()
	{
		SubLevel = "Classic01";
		Score = 0;
		Lives = 3;
		CherryNumber = 0;
	}

	FClassicData(const FName& SubLevelName, int32 ScoreNum, int32 LivesNum, int32 CherryNum)
		: SubLevel(SubLevelName), Score(ScoreNum), Lives(LivesNum), CherryNumber(CherryNum)
	{}
};

USTRUCT()
struct FMazeData
{
	GENERATED_BODY()

	FName SubLevel;
	int32 Score;
	int32 Lives;
	int32 CherryNumber;

	FMazeData()
	{
		SubLevel = "Maze01";
		Score = 0;
		Lives = 3;
		CherryNumber = 0;
	}

	FMazeData(const FName& SubLevelName, int32 ScoreNum, int32 LivesNum, int32 CherryNum)
		: SubLevel(SubLevelName), Score(ScoreNum), Lives(LivesNum), CherryNumber(CherryNum)
	{}
};

USTRUCT()
struct FScoreboardData
{
	GENERATED_BODY()

	int32 Score;
	int32 Cherries;
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

	void SetLevel(ELevelType LevelType) { CurrentLevelType = LevelType; };
	ELevelType GetCurrentLevel() const { return CurrentLevelType; }

	void SetClassicDara(const FClassicData& Data) { ClassicData = Data; }
	FClassicData GetClassicData() const { return ClassicData; }

	void SetMazeData(const FMazeData& Data) { MazeData = Data; }
	FMazeData GetMazeData() const { return  MazeData; } 

	FText GetScoreData(ELevelType LevelType) const;

	void AddScore(ELevelType LevelType, const FScoreboardData& ScoreData);

private:
	FText MakeScoreDataAsText(const TArray<FScoreboardData>& ScoreData) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan | Camera")
	TSubclassOf<APMCamera> ClassicCameraClass;

	UPROPERTY()
	ELevelType CurrentLevelType;
	
	UPROPERTY()
	FClassicData ClassicData;

	UPROPERTY()
	FMazeData MazeData;

	UPROPERTY()
	TArray<FScoreboardData> ClassicScoreData;
	UPROPERTY()
	TArray<FScoreboardData> MazeScoreData;
};
