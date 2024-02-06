// Fill out your copyright notice in the Description page of Project Settings.

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
	APMSpline* UPWARD;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "DOWN"))
	APMSpline* DOWN;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "LEFT"))
	APMSpline* LEFT;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "RIGHT"))
	APMSpline* RIGHT;

	FSplines()
		: UPWARD(nullptr), DOWN(nullptr), LEFT(nullptr), RIGHT(nullptr)
	{}
};

UCLASS()
class PACMAN_API APMSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APMSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSplines> Splines;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
	float Distance = 120.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APMCoin> CoinClass;
};
