// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMCoin.h"
#include "PMMapCoin.generated.h"

/**
 * 
 */
UCLASS()
class PACMAN_API APMMapCoin : public APMCoin
{
	GENERATED_BODY()

protected:
	/** Collecting map coins is not required to pass a level */
	virtual void IncreaseCoinsCounter() override {};

public:
	virtual int32 Interaction() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> PickUpSound;
};
