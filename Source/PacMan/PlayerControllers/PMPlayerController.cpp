// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMPlayerController.h"
#include "Cameras/PMCamera.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/PMGameInstance.h"

void APMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	SetCamera();
}

void APMPlayerController::SetCamera()
{
	if (ClassicCameraClass == nullptr)
	{
		return;
	}

	if (UPMGameInstance::GetCurrentLevelType() == ELevelType::CLASSIC)
	{
		AActor* Camera = Cast<APMCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), ClassicCameraClass));
		if (Camera != nullptr)
		{
			SetViewTargetWithBlend(Camera);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APMPlayerController::SetCamera | Camera is nullptr"));
		}
	}
	
}
