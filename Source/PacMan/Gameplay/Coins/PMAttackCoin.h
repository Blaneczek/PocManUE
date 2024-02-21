// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PMCoin.h"
#include "PMAttackCoin.generated.h"

/**
 * 
 */
UCLASS()
class PACMAN_API APMAttackCoin : public APMCoin
{
	GENERATED_BODY()

public:
	virtual int32 Interaction() override;
};
