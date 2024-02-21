// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PMInteractionInterface.h"
#include "PMCoin.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UMaterialInstance;

UCLASS()
class PACMAN_API APMCoin : public AActor, public IPMInteractionInterface
{
	GENERATED_BODY()

public:
	APMCoin();

protected:
	virtual void BeginPlay() override;

	virtual void IncreaseCoinsCounter();

public:

	virtual int32 Interaction() override;

	FORCEINLINE static int32 GetCoinsNumber() { return CoinsCounter; }
	static void ResetCoinsNumber() { CoinsCounter = 0; }

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Material")
	TObjectPtr<UMaterialInstance> ClassicMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Material")
	TObjectPtr<UMaterialInstance> MazeMaterial;

protected:
	static int32 CoinsCounter;
};
