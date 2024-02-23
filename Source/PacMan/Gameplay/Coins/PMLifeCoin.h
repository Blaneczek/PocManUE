// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Coins/PMCoin.h"
#include "PMLifeCoin.generated.h"

/**
 * 
 */
UCLASS()
class PACMAN_API APMLifeCoin : public APMCoin
{
	GENERATED_BODY()

protected:
	virtual void IncreaseCoinsCounter() override {};

public:
	virtual int32 Interaction() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> PickUpSound;
};
