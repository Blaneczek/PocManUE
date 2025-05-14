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
	HIT			UMETA(DisplayName = "Hit")
};

/** For the BFS algorithm in the search for the target spline  */
USTRUCT()
struct FSplineQueueData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 FirstSplineIndex = 0;
	UPROPERTY()
	int32 CurrentSplineIndex = 0;
	UPROPERTY()
	TObjectPtr<APMSpline> Spline = nullptr;
	
	FSplineQueueData()
	{}
	
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

private:
	/** Movement */
	bool CheckIfAtPoint();
	void HandleMovement();
	int32 FindPath(const FName& SplineTag);
	TArray<int32> FindValidSplinesInRandomMovement();
	TMap<int32, APMSpline*> FindValidSplinesInMarkedMovement(APMSpline* Spline, int32 Index);
	void MoveToNewSpline(float Direction, float YawRotation, APMSpline* NewSpline = nullptr);
	void GhostBaseMovement();
	void ChooseNewSpline(int32 ChosenSpline);
	void ReachingMarkedSpline();
	void TurnAround();	

	UFUNCTION()
	void OnSeePawn(APawn* OtherPawn);

	void AttackTimer();
	void CanSee();

	/** Sets proper speed and state to free Ghost from the base. */
	void Release();

	/** Resets all timers and needed variables. */
	void Reset();

	/** Sets proper values and states to prepare Ghost for the start of the game. */
	void Start();
	
	void StartMovement();
	void StopMovement();
	
	/** Starts vulnerable state where Ghost can be hit by the player. */
	void VulnerableState();
	
	void EndVulnerableState();
	void VulnerableFlickering();

	/** Sends Ghost back to base after getting hit by the player by setting proper state. */
	void BackToBase();
	
	void InitializeMaterial();
	void BindGameModeDelegates();
	
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> CollisionSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPawnSensingComponent> PawnSensing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UWidgetComponent> PointsWidget;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float NormalSpeed = 800.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float VulnerableSpeed = 600.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float ReturnSpeed = 1200.f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float ReleaseTime = 2.f;

	
	UPROPERTY()
	TObjectPtr<APMSpline> CurrentSpline;
	
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
	UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> HitSoundClassic;
	
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> HitSoundMaze;

	float Speed;
	float PositionOnSpline;
	float MovingDirection;
	int32 SplineIndex;
	int32 ChaseTimeCounter;
	bool bIsMoving;
	bool bFlickering;

	/** To call Interaction function only once when overlap with player */
	bool bDoOnce;

	/** To turn off the ghost's sight and give the player a chance to escape */
	bool bCanSee; 

	/** Ghost has 10% chance to turn around but if he didn't do it previously */
	bool bTurnedAround;
	
	FTimerHandle ChaseTimer;
	FTimerHandle CanSeeTimer;
	FTimerHandle ReleaseTimer;
	FTimerHandle VulnerableTimer;
	FTimerHandle FlickeringTimer;
};
