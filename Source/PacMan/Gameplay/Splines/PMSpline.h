// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMSpline.generated.h"

class USplineComponent;
class USceneComponent;
class APMCoin;

/** 
* Each spline has 2 points to which we can assign up to 4 other splines depending on which direction they should lead. 
* Such a grid of interconnected splines on a level allows the player and ghosts to move around the map.
* Setting splines is done on instance. A script is used for this: look at BP_Spline.
* Splines must overlap with each other and be in a relationship where Spline component point index 0 is the beginning and Spline component point index 1 is the end. 
*         1
*         | UPWARD
*		  |
*  LEFT   0  RIGHT
* 0------1|0------1
*		  1
*         |
*         | DOWN          
*         0
*/
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

	/** Splines[0] is for Spline component point index 0, Splines[1] for Spline component point index 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PocMan")
	TArray<FSplines> Splines;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneComponent;

private:
	/**
	* Distance between coins on spline. 
	* To maintain uniform coin spacing on all splines, the length of each spline must be a multiple of the number CoinDistanceOnSpline.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	float CoinDistanceOnSpline = 120.f;

	/** Spawn different coins depending on the type of game */
	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	TSubclassOf<APMCoin> ClassicCoinClass;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan")
	TSubclassOf<APMCoin> MazeCoinClass;
};
