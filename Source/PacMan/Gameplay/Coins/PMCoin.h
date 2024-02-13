// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PMInteractionInterface.h"
#include "PMCoin.generated.h"

class USphereComponent;
class UStaticMeshComponent;

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int32 Interaction() override;

public:
	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

};
