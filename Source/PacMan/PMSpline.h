// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PMSpline.generated.h"


USTRUCT(BlueprintType)
struct FSplines
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "UP"))
	TObjectPtr<APMSpline> UPWARD;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "DOWN"))
	TObjectPtr<APMSpline> DOWN;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "LEFT"))
	TObjectPtr<APMSpline> LEFT;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta = (DisplayName = "RIGHT"))
	TObjectPtr<APMSpline> RIGHT;

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

public:		
	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere)
	TArray<FSplines> Splines;

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;
};
