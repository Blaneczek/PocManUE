// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Player/PMPlayer.h"
#include "PMMazePlayer.generated.h"

class UInputAction;
class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class PACMAN_API APMMazePlayer : public APMPlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APMMazePlayer();

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void TurnAround();

	virtual void MoveLeft() override;
	virtual void MoveRight() override;

protected:
	//MappingContext
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* TurnAroundAction;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> CameraArm;
};