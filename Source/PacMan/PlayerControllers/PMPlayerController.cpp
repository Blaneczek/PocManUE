// Fill out your copyright notice in the Description page of Project Settings.


#include "PMPlayerController.h"
#include "Cameras/PMCamera.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/PMGameInstance.h"

void APMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
	SetCamera(GetCameraClass());
}

TSubclassOf<APMCamera> APMPlayerController::GetCameraClass() const
{
	UPMGameInstance* gameInstance = Cast<UPMGameInstance>(GetGameInstance());

	if (gameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayerController::GetCameraClass | GameInstance is nullptr"));
		return nullptr;
	}

	return gameInstance->GetCameraClass();
}	

void APMPlayerController::SetCamera(TSubclassOf<APMCamera> CameraClass)
{
	if (CameraClass == nullptr)
	{
		return;
	}

	AActor* camera = Cast<APMCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), CameraClass));
	if (camera != nullptr)
	{
		SetViewTargetWithBlend(camera);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayerController::SetCamera | Camera is nullptr"));
	}
}
