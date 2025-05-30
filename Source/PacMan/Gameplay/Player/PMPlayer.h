// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMPlayer.generated.h"

class USphereComponent;
class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class APMSpline;
class APMGameModeBase;

UENUM()
enum class EPlayerDirection : uint8
{
	NONE	UMETA(DisplayName = "None"),
	UPWARD	UMETA(DisplayName = "UP"),
	DOWN	UMETA(DisplayName = "DOWN"),
	LEFT	UMETA(DisplayName = "LEFT"),
	RIGHT	UMETA(DisplayName = "RIGHT")
};

UCLASS()
class PACMAN_API APMPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APMPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** It gives the current Spline a Tag so that the chasing ghosts know where to go. */
	void MarkSpline();
	void UnmarkSpline();

protected:
	UFUNCTION()
	virtual void MoveLeft() {};
	UFUNCTION()
	virtual void MoveRight() {};
	UFUNCTION()
	void OpenPauseMenu();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool CheckIfAtSplinePoint();
	
	/** Decides in which direction to go next when player is at the Spline point */
	void ChooseNewSpline();

	void Rotate180(EPlayerDirection Direction);	
	void Reset();
	void Start();
	void StartMovement();
	void StopMovement();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputMappingContext> MappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> MoveLeftAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> MoveRightAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> OpenPauseMenuAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float Speed = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	FRotator StartingRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float StartingMovingDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	EPlayerDirection StartingDirection;

	UPROPERTY()
	TObjectPtr<APMSpline> CurrentSpline;
	UPROPERTY()
	TObjectPtr<APMSpline> StartingSpline;

	UPROPERTY()
	TObjectPtr<APMGameModeBase> GameMode;

	EPlayerDirection DesiredDirection;
	EPlayerDirection CurrentDirection;

	/** Used when a player chooses a direction that is not available in the next ChooseNewSpline so it can turn to Desired direction in the next possible situation */
	EPlayerDirection TempDirection;

	float PositionOnSpline;
	float MovingDirection;
	bool bIsMoving;
	int32 SplineIndex;
};
