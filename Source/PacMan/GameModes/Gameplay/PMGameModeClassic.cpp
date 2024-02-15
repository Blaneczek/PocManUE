// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGameModeClassic.h"
#include "UI/HUD/PMClassicHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Gameplay/Ghosts/PMGhost.h"

APMGameModeClassic::APMGameModeClassic()
{

}

void APMGameModeClassic::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("classic game mode"));
	GhostAudio = UGameplayStatics::CreateSound2D(this, GhostSound, 1.f, 1.f, 0.f, nullptr, false, false);

}

void APMGameModeClassic::InitializeWidgets(APlayerController* PlayerController)
{
	Super::InitializeWidgets(PlayerController);

	if (ClassicHUDClass != nullptr)
	{
		HUDWidget = CreateWidget<UPMClassicHUD>(PlayerController, ClassicHUDClass);
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::InitializeWidgets | ClassicHUDClass is nullptr"));
	}
}


void APMGameModeClassic::StartAllMovement()
{
	Super::StartAllMovement();

	if (!IsValid(GhostAudio)) return;

	if (!GhostAudio->IsPlaying())
	{
		UE_LOG(LogTemp, Warning, TEXT("start sound"));
		GhostAudio->Play();
	}
}

void APMGameModeClassic::StopGame()
{
	Super::StopGame();

	if (!IsValid(GhostAudio)) return;

	if (GhostAudio->IsPlaying())
	{
		UE_LOG(LogTemp, Warning, TEXT("stop sound"));
		GhostAudio->Stop();
	}

}

void APMGameModeClassic::PlayerAttackState()
{
	Super::PlayerAttackState();

	if (!IsValid(GhostAudio)) return;

	GhostAudio->SetSound(VulnerableGhostSound);
}

void APMGameModeClassic::EndPlayerAttackState()
{
	Super::EndPlayerAttackState();

	for (auto ghost : Ghosts)
	{
		if (ghost->IsVulnerable()) return;
	}

	if (!IsValid(GhostAudio)) return;

	GhostAudio->SetSound(GhostSound);
}


