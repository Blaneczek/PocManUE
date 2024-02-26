// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMSpline.generated.h"

class USplineComponent;
class USceneComponent;
class APMCoin;

USTRUCT(BlueprintType)
struct FSplines
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "UP"))
	TObjectPtr<APMSpline> UPWARD;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "DOWN"))
	TObjectPtr<APMSpline>  DOWN;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "LEFT"))
	TObjectPtr<APMSpline>  LEFT;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "RIGHT"))
	TObjectPtr<APMSpline>  RIGHT;

	FSplines()
		: UPWARD(nullptr), DOWN(nullptr), LEFT(nullptr), RIGHT(nullptr)
	{}
};

UCLASS()
class PACMAN_API APMSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	APMSpline();

protected:
	virtual void BeginPlay() override;

public:
	void SpawnCoins();

public:		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PocMan")
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PocMan")
	TArray<FSplines> Splines;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	float CoinDistanceOnSpline = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	TSubclassOf<APMCoin> ClassicCoinClass;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	TSubclassOf<APMCoin> MazeCoinClass;
};
