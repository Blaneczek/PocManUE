// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PMInteractionInterface.h"
#include "PMGhost.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class APMSpline;
class APMGameModeBase;
class UPawnSensingComponent;
class APMPlayer;
class UMaterialInstanceDynamic;
class UWidgetComponent;

UENUM()
enum class EGhostState : uint8
{
	NONE		UMETA(DisplayName = "None"),
	VULNERABLE	UMETA(DisplayName = "Vulnerable")
};

UENUM()
enum class EGhostMovementState : uint8
{
	NONE		UMETA(DisplayName = "None"),
	PASSIVE		UMETA(DisplayName = "Passive"),
	ATTACK		UMETA(DisplayName = "Attack"),
	BASE		UMETA(DisplayName = "Base"),
	RELEASE		UMETA(DisplayName = "Release"),
	HITTED		UMETA(DisplayName = "Hitted")
};

USTRUCT()
struct FSplineQueueData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 FirstSplineIndex;
	UPROPERTY()
	int32 CurrentSplineIndex;
	UPROPERTY()
	TObjectPtr<APMSpline> Spline;

	FSplineQueueData()
	{
	}

	FSplineQueueData(int32 firstSpline, int32 currentSplineIndex, APMSpline* Spline)
		: FirstSplineIndex(firstSpline), CurrentSplineIndex(currentSplineIndex), Spline(Spline)
	{}
};

UCLASS()
class PACMAN_API APMGhost : public APawn, public IPMInteractionInterface
{
	GENERATED_BODY()

public:
	APMGhost();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int32 Interaction() override;

protected:
	virtual void SetEyesPosition(const int32 YawRotation) {};

	TArray<int32> FindValidSplinesInRandomMovement();
	bool CheckIfAtPoint();
	void HandleMovement();
	int32 FindPath(const FName& SplineTag);
	TMap<int32, APMSpline*> FindValidSplinesInMarkedMovement(APMSpline* Spline, int32 index);

	UFUNCTION()
	void OnSeePawn(APawn* OtherPawn);

	void AttackTimer();

	void Release();
	void Reset();
	void Start();
	void StartMovement();
	void StopMovement();

	void VulnerableState();
	void EndVulnerableState();
	void VulnerableFlickering();
	void BackToBase();
	void CanSee();

	bool IsVulnerable() { return bVulnerable; }

private:
	void InitializeMaterial();
	void BindGameModeDelegates();
	void MoveToNewSpline(APMSpline* NewSpline, float Direction, float YawRotation);
	void TurnAround();
	void GhostBaseMovement();
	void ChooseNewSpline(int32 ChoosenSpline);
	void ReachingMarkedSpline();
	void ClearVulnerableTimers();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> CollisionSphere;	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPawnSensingComponent> PawnSensing;	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UWidgetComponent> PointsWidget;
	UPROPERTY()
	TObjectPtr<APMSpline> CurrentSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float NormalSpeed = 800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float VulnerableSpeed = 600.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float ReturnSpeed = 1200.f;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float ReleaseTime = 2.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	TObjectPtr<APMSpline> StartingSpline;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	EGhostMovementState StartingMovementState;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	FRotator StartingRotation;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float StartingMovingDirection;	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	FLinearColor StartingColor;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	FLinearColor VulnerableColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	int32 MaxChaseTime = 4;

protected:
	UPROPERTY()
	TObjectPtr<APMGameModeBase> GameMode;

	EGhostState State;
	EGhostMovementState MovementState;

	UPROPERTY()
	TObjectPtr<APMPlayer> Player = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;

	// Audio
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> HitSoundClassic;
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> HitSoundMaze;
	//

	float Speed = 500.f;
	float PositionOnSpline;
	float MovingDirection;
	int32 SplineIndex;
	int32 ChaseTimeCounter;
	bool bIsMoving;
	bool bDoOnce;
	bool bVulnerable;
	bool bGhostHitted;
	bool bFlickering;
	bool bCanSee;
	

	// Timers
	FTimerHandle ChaseTimer;
	FTimerHandle CanSeeTimer;
	FTimerHandle ReleaseTimer;
	FTimerHandle VulnerableTimer;
	FTimerHandle FlickeringTimer;
	//
};
