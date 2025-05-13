// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGameModeMenu.h"
#include "GameInstance/PMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menu/PMMenuWidget.h"

void APMGameModeMenu::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPMGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::BeginPlay | GameInstance is nullptr"));
		return;
	}

	if (MenuAudio != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, MenuAudio, 0.5f);
	}

	GameInstance->LoadGame();
	InitializeMenu();
}

void APMGameModeMenu::ChooseNewGame(ELevelType GameType)
{
	if (!IsValid(GameInstance)) return;

	GameInstance->SetLevelType(GameType);
	FName LevelName;

	switch (GameType)
	{
		case ELevelType::CLASSIC:
		{
			GameInstance->ClassicGameData = FGameData(1, 0, 0);
			LevelName = *GameInstance->ClassicLevels.Find(1);
			break;
		}
		case ELevelType::MAZE:
		{
			GameInstance->MazeGameData = FGameData(1, 0, 0);
			LevelName = *GameInstance->MazeLevels.Find(1);
			break;
		}
		default: return;
	}

	GameInstance->SaveGame();
	UGameplayStatics::OpenLevel(this, LevelName);
}

void APMGameModeMenu::ContinueGame(ELevelType GameType)
{
	if (!IsValid(GameInstance)) return;

	GameInstance->SetLevelType(GameType);
	FName* LevelName;

	switch (GameType)
	{
		case ELevelType::CLASSIC:
		{
			LevelName = GameInstance->ClassicLevels.Find(GameInstance->ClassicGameData.LevelNum);	
			break;
		}
		case ELevelType::MAZE:
		{
			LevelName = GameInstance->MazeLevels.Find(GameInstance->MazeGameData.LevelNum);
			break;
		}
		default: return;
	}

	UGameplayStatics::OpenLevel(this, *LevelName);
}

void APMGameModeMenu::ExitGame()
{
	UKismetSystemLibrary::QuitGame(this, UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
}

void APMGameModeMenu::InitializeMenu()
{
	if (MenuWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::InitializeMenu | MenuWidgetClass is nullptr"));
		return;
	}

	UPMMenuWidget* MenuWidget = CreateWidget<UPMMenuWidget>(GetWorld(), MenuWidgetClass);
	if (!MenuWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGameModeMenu::InitializeMenu | MenuWidget is nullptr"));
		return;
	}

	MenuWidget->OnStartNewClassic.BindUObject(this, &APMGameModeMenu::ChooseNewGame);
	MenuWidget->OnStartNewMaze.BindUObject(this, &APMGameModeMenu::ChooseNewGame);
	MenuWidget->OnContinueClassic.BindUObject(this, &APMGameModeMenu::ContinueGame);
	MenuWidget->OnContinueMaze.BindUObject(this, &APMGameModeMenu::ContinueGame);
	MenuWidget->OnExitGame.BindUObject(this, &APMGameModeMenu::ExitGame);

	GameInstance->ClassicGameData.LevelNum == 1 ? MenuWidget->SetIsEnabledClassicButton(false) : MenuWidget->SetIsEnabledClassicButton(true);
	GameInstance->MazeGameData.LevelNum == 1 ? MenuWidget->SetIsEnabledMazeButton(false) : MenuWidget->SetIsEnabledMazeButton(true);

	MenuWidget->AddToViewport();
}






