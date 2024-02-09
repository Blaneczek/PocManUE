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
class UMaterialInstanceDynamic;

UENUM()
enum EGhostState : uint8
{
	PASSIVE	UMETA(DisplayName = "PASSIVE"),
	ATTACK	UMETA(DisplayName = "ATTACK"),
	WAIT	UMETA(DisplayName = "WAIT"),
	RELEASE UMETA(DisplayName = "RELEASE")
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
	void StopMovement();

	void VulnerableState();
	void EndVulnerableState();
	void VulnerableFlickering();
	void BackToBase();
	void CanSee();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphere;

	UPROPERTY()
	APMSpline* CurrentSpline;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;	

	UPROPERTY(EditDefaultsOnly)
	float NormalSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float VulnerableSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly)
	float ReturnSpeed = 1200.f;

	UPROPERTY(EditInstanceOnly)
	float ReleaseTime = 2.f;

	//maybe use delegate 
	FOnGhostHit OnGhostHitDelegate;

private:
	UPROPERTY()
	TEnumAsByte<EGhostState> State;

	UPROPERTY(EditInstanceOnly)
	TEnumAsByte<EGhostState> StartingState;

	UPROPERTY(EditInstanceOnly)
	FRotator StartingRotation;

	UPROPERTY(EditInstanceOnly)
	float StartingMovingDirection;

	UPROPERTY(EditInstanceOnly)
	APMSpline* StartingSpline;

	UPROPERTY()
	TObjectPtr<APMPlayer> Player = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxChaseTime = 4;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY(EditInstanceOnly)
	FLinearColor StartingColor;

	float Speed = 50.f;
	float PositionOnSpline;
	float MovingDirection;
	int32 SplineIndex;
	bool bIsMoving;
	bool bDoOnce;
	bool bVulnerable;
	bool bGhostHitted;
	bool bFlickering;
	bool bCanSee;
	int32 ChaseTimeCounter = 0;

	FTimerHandle ChaseTimerHandle;
	FTimerHandle CanSeeTimerHandle;
	FTimerHandle ReleaseTimerHandle;
	FTimerHandle VulnerableTimerHandle;
	FTimerHandle FlickeringTimerHandle;
};
