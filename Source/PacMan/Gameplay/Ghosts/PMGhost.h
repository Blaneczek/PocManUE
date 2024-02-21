// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

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
enum class EGhostState : uint8
{
	PASSIVE	UMETA(DisplayName = "PASSIVE"),
	ATTACK	UMETA(DisplayName = "ATTACK"),
	WAIT	UMETA(DisplayName = "WAIT"),
	RELEASE UMETA(DisplayName = "RELEASE")
};

UENUM()
enum class EGhostDirection : uint8
{
	NONE	UMETA(DisplayName = "None"),
	UPWARD	UMETA(DisplayName = "UP"),
	DOWN	UMETA(DisplayName = "DOWN"),
	LEFT	UMETA(DisplayName = "LEFT"),
	RIGHT	UMETA(DisplayName = "RIGHT")
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

	bool IsVulnerable() { return bVulnerable; }

protected:
	virtual void SetEyesPosition(const int32 YawRotation) {};

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY()
	TObjectPtr<APMSpline> CurrentSpline;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;	

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	float NormalSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	float VulnerableSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	float ReturnSpeed = 1200.f;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	float ReleaseTime = 2.f;


	UPROPERTY()
	EGhostDirection CurrentDirection;

private:
	UPROPERTY()
	EGhostState State;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	EGhostState StartingState;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	FRotator StartingRotation;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	float StartingMovingDirection;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	TObjectPtr<APMSpline> StartingSpline;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	FLinearColor StartingColor;

	UPROPERTY(EditInstanceOnly, Category = "PocMan|Gameplay")
	FLinearColor VulnerableColor;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Gameplay")
	int32 MaxChaseTime = 4;

	UPROPERTY()
	TObjectPtr<APMPlayer> Player = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;

	// Sounds
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> HitSoundClassic;

	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> HitSoundMaze;

	float Speed = 50.f;
	float PositionOnSpline;
	int32 MovingDirection;
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
