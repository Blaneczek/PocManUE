// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMCoin.h"
#include "PMMapCoin.generated.h"

class USoundWave;
/**
 * 
 */
UCLASS()
class PACMAN_API APMMapCoin : public APMCoin
{
	GENERATED_BODY()

public:
	virtual int32 Interaction() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PocMan|Sound")
	TObjectPtr<USoundWave> PickUpSound;
};
