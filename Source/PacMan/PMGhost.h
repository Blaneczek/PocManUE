// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PMInteractionInterface.h"
#include "PMGhost.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGhostHit);

class UStaticMeshComponent;
class USphereComponent;
class APMSpline;
class UPawnSensingComponent;
class APMPlayer;

UENUM()
enum EGhostState : uint8
{
	PASSIVE	UMETA(DisplayName = "PASSIVE"),
	ATTACK	UMETA(DisplayName = "ATTACK"),
	WAIT	UMETA(DisplayName = "WAIT")
};

USTRUCT()
struct FSplineQueueData
{
	GENERATED_BODY()

	int32 firstSpline;
	int32 currentSplineIndex;
	APMSpline* Spline;

	FSplineQueueData()
	{
	}

	FSplineQueueData(int32 firstSpline, int32 currentSplineIndex, APMSpline* Spline)
		: firstSpline(firstSpline), currentSplineIndex(currentSplineIndex), Spline(Spline)
	{}
};

UCLASS()
class PACMAN_API APMGhost : public APawn, public IPMInteractionInterface
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
	TMap<int32, APMSpline*> AvailableSplines(APMSpline* Spline, int32 index);

	UFUNCTION(BlueprintCallable)
	void OnSeePawn(APawn* OtherPawn);
	void AttackTimer();

	virtual int32 Interaction() override;

	void ReleaseGhost();
	void ResetGhost();
	void HideGhost();
	void StartGhost();
	void StartMovement();

	void VulnerableState();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* CollisionSphere;

	UPROPERTY()
	APMSpline* CurrentSpline;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UPawnSensingComponent> PawnSensing;	

	UPROPERTY(EditAnywhere)
	float Speed = 50.f;

	UPROPERTY(EditAnywhere)
	float VulnerableSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	float ReleaseTime = 2.f;

	//maybe use delegate 
	FOnGhostHit OnGhostHitDelegate;

private:
	UPROPERTY()
	TEnumAsByte<EGhostState> State;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGhostState> StartingState;

	UPROPERTY(EditAnywhere)
	FRotator StartingRotation;

	UPROPERTY(EditAnywhere)
	float StartingMovingDirection;

	UPROPERTY(EditAnywhere)
	APMSpline* StartingSpline;

	UPROPERTY()
	TObjectPtr<APMPlayer> Player = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxChaseTime = 2;

	UPROPERTY(EditAnywhere)
	FName GhostTag;

	float PositionOnSpline;
	float MovingDirection;
	bool bIsMoving;
	int32 SplineIndex;
	bool bDoOnce;

	FTimerHandle ChaseTimerHandle;
	int32 ChaseTimeCounter = 0;

	FTimerHandle ReleaseTimerHandle;

};
