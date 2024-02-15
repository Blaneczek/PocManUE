// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMGhost.h"
#include "PMMazeGhost.generated.h"

class USoundCue;
class UAudioComponent;

/**
 * 
 */
UCLASS()
class PACMAN_API APMMazeGhost : public APMGhost
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundCue> MovementCue;

	UPROPERTY()
	TObjectPtr<UAudioComponent> GhostAC;
};
