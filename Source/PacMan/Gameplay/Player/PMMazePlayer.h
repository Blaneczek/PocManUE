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
	UFUNCTION()
	void OpenMap();

	virtual void MoveLeft() override;
	virtual void MoveRight() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> TurnAroundAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> OpenMapAction;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> CameraArm;
};
