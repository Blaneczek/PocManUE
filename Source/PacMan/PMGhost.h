// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMGhost.generated.h"

class UStaticMeshComponent;
class APMSpline;
class UPawnSensingComponent;

UENUM()
enum EGhostState : uint8
{
	NONE	UMETA(DisplayName = "NONE"),
	PASSIVE	UMETA(DisplayName = "PASSIVE"),
	ATTACK	UMETA(DisplayName = "ATTACK")
};

UCLASS()
class PACMAN_API APMGhost : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APMGhost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CheckIfAtPoint();

	void ChooseNewSpline();

	UFUNCTION()
	void OnSeePawn(APawn* OtherPawn);

public:
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	APMSpline* CurrentSpline;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UPawnSensingComponent> PawnSensing;	

	UPROPERTY(EditAnywhere)
	float Speed = 50.f;

private:
	UPROPERTY()
	TEnumAsByte<EGhostState> State;

	float PositionOnSpline;
	float MovingDirection;
	bool bIsMoving;
	int32 SplineIndex;
};
