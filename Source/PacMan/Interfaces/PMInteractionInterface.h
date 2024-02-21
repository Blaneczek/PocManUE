// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PMInteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPMInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACMAN_API IPMInteractionInterface
{
	GENERATED_BODY()

public:

	virtual int32 Interaction();

};
