// Fill out your copyright notice in the Description page of Project Settings.


#include "PMGameModeMenu.h"
#include "GameInstance/PMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UI/Menu/PMMenuWidget.h"
#include "PlayerControllers/PMPlayerControllerMenu.h"

void APMGameModeMenu::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPMGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::BeginPlay | GameInstance is nullptr"));
	}

	InitializeMenu();

}

void APMGameModeMenu::ChooseGameType(ELevelType GameType)
{
	UE_LOG(LogTemp, Warning, TEXT("nie ma yupu"));
	GameInstance->SetLevel(GameType);

	switch (GameType)
	{
		case ELevelType::CLASSIC:
		{
			UGameplayStatics::OpenLevel(this, "Classic");
			UE_LOG(LogTemp, Warning, TEXT("halo gdzie level"));
			return;
		}
		case ELevelType::LABIRYNTH :
		{
			UGameplayStatics::OpenLevel(this, "Labirynth");
			return;
		}
		default: return;
	}
}

void APMGameModeMenu::InitializeMenu()
{
	if (MenuWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::InitializeMenu | MenuWidgetClass is nullptr"));
		return;
	}

	APMPlayerControllerMenu* PCM = Cast<APMPlayerControllerMenu>(UGameplayStatics::GetPlayerController(this, 0));
	if (PCM == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::InitializeMenu | PCM is nullptr"));
		return;
	}
	MenuWidget = CreateWidget<UPMMenuWidget>(PCM, MenuWidgetClass);
	MenuWidget->AddToViewport();
}




