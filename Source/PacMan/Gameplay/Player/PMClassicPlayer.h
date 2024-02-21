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
	APMClassicPlayer();

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveUp();
	UFUNCTION()
	void MoveDown();

	virtual void MoveLeft() override;
	virtual void MoveRight() override;

private:
	void AnimateMesh(float DeltaTime);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> MoveUpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> MoveDownAction;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)\
	TObjectPtr<UStaticMeshComponent> TopMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> BottomMesh;

private:
	float AnimationSpeed;
	float AnimationTimeElapsed = 0.f;
	float AnimationDirection;
};
