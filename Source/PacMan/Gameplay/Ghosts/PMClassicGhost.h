// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Ghosts/PMGhost.h"
#include "PMClassicGhost.generated.h"

class UStaticMeshComponent;
class USceneComponent;

/**
 * 
 */
UCLASS()
class PACMAN_API APMClassicGhost : public APMGhost
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APMClassicGhost();

protected:
	virtual void SetEyesPosition(const int32 YawRotation) override;

private:
	void SetEyesRight();
	void SetEyesLeft();
	void SetEyesUp();
	void SetEyesDown();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> DummyScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> EyeR;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> EyeL;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> IrisR;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> IrisL;
};
