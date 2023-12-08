// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMGhost.generated.h"

class UStaticMeshComponent;
class APMSpline;
class UPawnSensingComponent;
class APMPlayer;

UENUM()
enum EGhostState : uint8
{
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

	int32 FindPath();

	UFUNCTION()
	void OnSeePawn(APawn* OtherPawn);

	TMap<int32, APMSpline*> AvailableSplines(APMSpline* Spline);
	TMap<int32, APMSpline*> AvailableSplines2(APMSpline* Spline, int32 index);

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

	UPROPERTY()
	TObjectPtr<APMPlayer> Player = nullptr;

	float PositionOnSpline;
	float MovingDirection;
	bool bIsMoving;
	int32 SplineIndex;
};
