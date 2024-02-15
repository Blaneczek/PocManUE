// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMMazeGhost.h"
#include "Sound//SoundCue.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void APMMazeGhost::BeginPlay()
{
	Super::BeginPlay();

	if (MovementCue != nullptr)
	{
		GhostAC = UGameplayStatics::SpawnSoundAttached(MovementCue, GetRootComponent());
	}
}
