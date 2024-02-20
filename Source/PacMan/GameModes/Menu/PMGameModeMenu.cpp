// Fill out your copyright notice in the Description page of Project Settings.


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

	InitializeMenu();
}

void APMGameModeMenu::ChooseNewGame(ELevelType GameType)
{
	GameInstance->SetLevelType(GameType);

	switch (GameType)
	{
		case ELevelType::CLASSIC:
		{
			GameInstance->ClassicGameData = FGameData(1, 0, 0);
			UGameplayStatics::OpenLevel(this, *GameInstance->ClassicLevels.Find(1));
			return;
		}
		case ELevelType::MAZE:
		{
			GameInstance->MazeGameData = FGameData(1, 0, 0);
			UGameplayStatics::OpenLevel(this, *GameInstance->MazeLevels.Find(1));
			return;
		}
		default: return;
	}
}

void APMGameModeMenu::ContinueGame(ELevelType GameType)
{
	GameInstance->SetLevelType(GameType);

	switch (GameType)
	{
		case ELevelType::CLASSIC:
		{
			if (GameInstance->ClassicGameData.LevelNum != 1)
			{
				UGameplayStatics::OpenLevel(this, *GameInstance->ClassicLevels.Find(GameInstance->ClassicGameData.LevelNum));
			}			
			return;
		}
		case ELevelType::MAZE:
		{
			if (GameInstance->MazeGameData.LevelNum != 1)
			{
				UGameplayStatics::OpenLevel(this, *GameInstance->MazeLevels.Find(GameInstance->MazeGameData.LevelNum));
			}
			return;
		}
	default: return;
	}
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

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		MenuWidget = CreateWidget<UPMMenuWidget>(PC, MenuWidgetClass);
		MenuWidget->OnStartNewClassic.BindUObject(this, &APMGameModeMenu::ChooseNewGame);
		MenuWidget->OnStartNewMaze.BindUObject(this, &APMGameModeMenu::ChooseNewGame);
		MenuWidget->OnContinueClassic.BindUObject(this, &APMGameModeMenu::ContinueGame);
		MenuWidget->OnContinueMaze.BindUObject(this, &APMGameModeMenu::ContinueGame);
		MenuWidget->OnExitGame.BindUObject(this, &APMGameModeMenu::ExitGame);

		GameInstance->ClassicGameData.LevelNum == 1 ? MenuWidget->SetIsEnabledClassicButton(false) : MenuWidget->SetIsEnabledClassicButton(true);
		GameInstance->MazeGameData.LevelNum == 1 ? MenuWidget->SetIsEnabledMazeButton(false) : MenuWidget->SetIsEnabledMazeButton(true);

		MenuWidget->AddToViewport();
	}
}




