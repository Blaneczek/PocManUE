// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMCoin.h"
#include "PMCherryCoin.generated.h"


/**
 * 
 */
UCLASS()
class PACMAN_API APMCherryCoin : public APMCoin
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
