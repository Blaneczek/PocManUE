// Fill out your copyright notice in the Description page of Project Settings.

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

public:
	TSubclassOf<APMCamera> GetCameraClass() const;
	void SetCamera(TSubclassOf<APMCamera> CameraClass);

};
