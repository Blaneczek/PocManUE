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
	// Sets default values for this actor's properties
	APMCoin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetMaterial();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int32 Interaction() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Material")
	TObjectPtr<UMaterialInstance> ClassicMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Material")
	TObjectPtr<UMaterialInstance> MazeMaterial;
};
