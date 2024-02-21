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

	if (UPMGameInstance* GameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		SetCamera(GameInstance->GetCameraClass());
	}	
}

void APMPlayerController::SetCamera(TSubclassOf<APMCamera> CameraClass)
{
	if (CameraClass == nullptr)
	{
		return;
	}

	AActor* Camera = Cast<APMCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), CameraClass));
	if (Camera != nullptr)
	{
		SetViewTargetWithBlend(Camera);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayerController::SetCamera | Camera is nullptr"));
	}
}
