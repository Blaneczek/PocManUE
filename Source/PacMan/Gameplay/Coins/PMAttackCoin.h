// Fill out your copyright notice in the Description page of Project Settings.

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
