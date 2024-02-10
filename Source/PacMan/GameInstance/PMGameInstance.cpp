// Fill out your copyright notice in the Description page of Project Settings.

#include "PMGameInstance.h"

void UPMGameInstance::Init()
{
	Super::Init();

	SetLevel(ELevelType::MENU);
}

TSubclassOf<APMCamera> UPMGameInstance::GetCameraClass() const
{
	switch (CurrentLevel)
	{
		case ELevelType::CLASSIC: return ClassicCameraClass;
		case ELevelType::LABIRYNTH: return LabirynthCameraClass;
	}
	return nullptr;
}

