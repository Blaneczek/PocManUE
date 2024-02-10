// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMPlayer.generated.h"

class USphereComponent;
class UInputMappingContext;
class UInputAction;
class UInputAction;
class UStaticMeshComponent;
class APMSpline;
class APMGameModeBase;

UENUM()
enum EDirection : uint8
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveUpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveDownAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveLeftAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveRightAction;

	
public:
	UFUNCTION()
	void MoveUp();
	UFUNCTION()
	void MoveDown();
	UFUNCTION()
	void MoveLeft();
	UFUNCTION()
	void MoveRight();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool CheckIfAtPoint();
	void ChooseNewSpline();

	void RotatePlayer(float Yaw, TEnumAsByte<EDirection> Direction);

	void MarkSpline();
	void UnmarkSpline();

	void ResetPlayer();
	void HidePlayer();
	void StartPlayer();
	void StartMovement();
	void StopMovement();

public:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	APMSpline* CurrentSpline;

	UPROPERTY(EditAnywhere)
	float Speed = 50.f;

	UPROPERTY()
	TEnumAsByte<EDirection> DesiredDirection;

	UPROPERTY()
	TEnumAsByte<EDirection> CurrentDirection;

private:
	UPROPERTY(EditAnywhere)
	FRotator StartingRotation;

	UPROPERTY(EditAnywhere)
	float StartingMovingDirection;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDirection> StartingDirection;

	APMGameModeBase* GameMode = nullptr;
	float PositionOnSpline;
	float MovingDirection;
	bool bIsMoving;
	int32 SplineIndex;
	EDirection TempDirection;
	bool bChased;
};
