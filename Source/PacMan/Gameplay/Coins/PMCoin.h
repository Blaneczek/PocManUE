// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PMInteractionInterface.h"
#include "PMCoin.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UMaterialInstance;
class USoundWave;

UCLASS()
class PACMAN_API APMCoin : public AActor, public IPMInteractionInterface
{
	GENERATED_BODY()

public:
	APMCoin();

protected:
	virtual void BeginPlay() override;

	/** 
	* Collecting all regular and attack coin is required to pass the level. 
	* The static variable increases when these coins are spawned and decreases after they are collected. 
	*/
	virtual void IncreaseCoinsCounter();

public:

	virtual int32 Interaction() override;

	FORCEINLINE static int32 GetCoinsNumber() { return CoinsCounter; }

	/** When moving to the next level or switching the game mode, the static variable must be reset. */
	static void ResetCoinsNumber() { CoinsCounter = 0; }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;

protected:
	static int32 CoinsCounter;
};
