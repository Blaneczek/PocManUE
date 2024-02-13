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
		return;
	}

	InitializeMenu();

}

void APMGameModeMenu::ChooseGame(ELevelType GameType)
{
	GameInstance->SetLevel(GameType);

	switch (GameType)
	{
		case ELevelType::CLASSIC:
		{
			UGameplayStatics::OpenLevel(this, "Classic");
			return;
		}
		case ELevelType::MAZE:
		{
			UGameplayStatics::OpenLevel(this, "Maze");
			return;
		}
		default: return;
	}
}

void APMGameModeMenu::ContinueGame(ELevelType GameType)
{

}

void APMGameModeMenu::ExitGame()
{
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(this, 0), EQuitPreference::Quit, true);
}

void APMGameModeMenu::InitializeMenu()
{
	if (MenuWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::InitializeMenu | MenuWidgetClass is nullptr"));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC != nullptr)
	{
		MenuWidget = CreateWidget<UPMMenuWidget>(PC, MenuWidgetClass);
		MenuWidget->OnStartNewClassic.AddDynamic(this, &APMGameModeMenu::ChooseGame);
		MenuWidget->OnStartNewMaze.AddDynamic(this, &APMGameModeMenu::ChooseGame);
		MenuWidget->OnContinueClassic.AddDynamic(this, &APMGameModeMenu::ContinueGame);
		MenuWidget->OnContinueMaze.AddDynamic(this, &APMGameModeMenu::ContinueGame);
		MenuWidget->OnExitGame.AddDynamic(this, &APMGameModeMenu::ExitGame);
		MenuWidget->AddToViewport();
	}
}




