// Fill out your copyright notice in the Description page of Project Settings.

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
enum class EDirection : uint8
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//MappingContext
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	UInputAction* MoveLeftAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	UInputAction* MoveRightAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	UInputAction* OpenPauseMenuAction;

	UFUNCTION()
	virtual void MoveLeft() {};
	UFUNCTION()
	virtual void MoveRight() {};
	UFUNCTION()
	void OpenPauseMenu();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool CheckIfAtPoint();
	void ChooseNewSpline();

	void RotatePlayer(float Yaw, EDirection Direction);	

public:
	void HidePlayer();
	void ResetPlayer();
	void StartPlayer();
	void StartMovement();
	void StopMovement();
	void MarkSpline();
	void UnmarkSpline();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

protected:
	UPROPERTY()
	APMSpline* CurrentSpline;

	UPROPERTY()
	TObjectPtr<APMGameModeBase> GameMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float Speed = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	FRotator StartingRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	float StartingMovingDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PocMan|Gameplay")
	EDirection StartingDirection;

	EDirection DesiredDirection;
	EDirection CurrentDirection;
	EDirection TempDirection;

	float PositionOnSpline;
	float MovingDirection;
	bool bIsMoving;
	int32 SplineIndex;
};
