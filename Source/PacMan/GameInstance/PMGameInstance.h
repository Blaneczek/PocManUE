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

	void SetLevel(ELevelType Level) { CurrentLevel = Level; };

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan | Camera")
	TSubclassOf<APMCamera> ClassicCameraClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan | Camera")
	TSubclassOf<APMCamera> MazeCameraClass;

	UPROPERTY()
	ELevelType CurrentLevel;

	//Classic gameplay data
	int32 ClassicScore;
	int32 ClssicLives;
	int32 ClassicCherryNumber;

	//Maze gameplay data

};
