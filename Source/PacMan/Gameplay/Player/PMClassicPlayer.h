// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Player/PMPlayer.h"
#include "PMClassicPlayer.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class PACMAN_API APMClassicPlayer : public APMPlayer
{
	GENERATED_BODY()

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveUp();
	UFUNCTION()
	void MoveDown();

	virtual void MoveLeft() override;
	virtual void MoveRight() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	UInputAction* MoveUpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	UInputAction* MoveDownAction;
};
