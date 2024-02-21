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

protected:
	virtual void SetMaterial() override;

public:
	virtual int32 Interaction() override;
};
