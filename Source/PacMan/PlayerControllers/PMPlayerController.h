// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PMPlayerController.generated.h"

class APMCamera;

/**
 * 
 */
UCLASS()
class PACMAN_API APMPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetCamera(TSubclassOf<APMCamera> CameraClass);

};
