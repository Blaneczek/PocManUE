// Fill out your copyright notice in the Description page of Project Settings.


#include "PMPlayerController.h"
#include "PMCamera.h"
#include "Kismet/GameplayStatics.h"

void APMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	AActor* Camera = Cast<APMCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), CameraClass));
	if (Camera != nullptr)
	{
		SetViewTargetWithBlend(Camera);
	}	
}
