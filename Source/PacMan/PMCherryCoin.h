// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PMCoin.h"
#include "PMCherryCoin.generated.h"

class APMSpline;
/**
 * 
 */
UCLASS()
class PACMAN_API APMCherryCoin : public APMCoin
{
	GENERATED_BODY()

public:
	virtual int32 Interaction() override;

};
