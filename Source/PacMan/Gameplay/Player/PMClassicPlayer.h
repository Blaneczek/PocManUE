// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMPlayer.h"
#include "PMClassicPlayer.generated.h"


/**
 * 
 */
UCLASS()
class PACMAN_API APMClassicPlayer : public APMPlayer
{
	GENERATED_BODY()
public:
	APMClassicPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveUp();
	UFUNCTION()
	void MoveDown();

	virtual void MoveLeft() override;
	virtual void MoveRight() override;

private:
	void AnimateMesh(float DeltaTime);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> TopMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> BottomMesh;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> MoveUpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PocMan|Enhanced Input")
	TObjectPtr<UInputAction> MoveDownAction;

private:
	float AnimationSpeed;
	float AnimationTimeElapsed = 0.f;
	float AnimationDirection;
};
