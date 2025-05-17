# Pocman
Pocman is a 2-in-1 game, with the well-known classic version and something new, 3D Maze. In this unique variant you'll have to strain all your senses to get through the complicated maze.

![classic](https://github.com/user-attachments/assets/ce96ebc4-ab6e-4dd2-b071-04a50cc9f48d) ![maze](https://github.com/user-attachments/assets/fdb5eef9-ad6c-4365-8f17-94dc7729361c)

# Introduction
Pocman is a complete game developed in Unreal Engine 5 and C++ as part of my portfolio. It allows you to play in 2 different modes and has spline based movement.

# Playable game
The game is fully playable. The build can be downloaded on the itchi.io platform:
https://blaneczek.itch.io/pocman

# Key elements

|                                                                               | Description                                                     |
|-------------------------------------------------------------------------------|-----------------------------------------------------------------|
| [Game modes](#game-modes-code)                                                | Managing the flow of gameplay in classic and maze mode          |
| [Splines](#splines-code)                                                      | Splines system on which movement and coins spawn are based.     |
| [Movement](#movement-code-playercode-ghost)                                   | Detailed description of the movement.                           |
| [Ghosts](#ghosts-code)                                                        | How ghosts work and how they behave.                            |
| [Coins](#coins-code)                                                          | Coin types and implementation.                                  |

# Game modes ([code](Source/PacMan/GameModes/Gameplay))    

<details>
<summary>More</summary>
The rules and gameplay are managed by the game modes. The base class contains all functions that are shared between modes and virtual functions that are ready to be overwritten.
  
```c++
class PACMAN_API APMGameModeBase : public AGameModeBase
{
...
public:
	/** When a player is hit by any ghost */
	virtual void HandleGhostHit();

	/** When a player collects attack coin */
	virtual void StartPlayerAttackState();
	virtual void EndPlayerAttackState() {};

	/** When any ghost sees the player or loses sight of the player */
	virtual void SetPlayerChased(bool IsPlayerChased) {};

	void AddCherryCoin();
	void AddPoints(int32 Points);
	void StopAllMovement();
	void OpenPauseMenu();

protected:
	/** Initializes widgets added to viewport at the beginning that will be used many times later in the game */
	virtual void InitStartingWidgets();

	/** Sets gameplay data from GameInstance depending on the game mode */
	virtual void SetGameplayValues() {};
...
```
In BeginPlay we cache the game instance pointer, as it will be used often later. Then we start the music, set all the necessary values, create widgets and set timers to start the game at the right time.

```c++
void APMGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPMGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGameModeBase::BeginPlay | GameInstance is nullptr"));
		return;
	}
	
	if (IsValid(GameMusic))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GameMusic);
	}

	SetGameplayValues();
	SetSplines();
		
	InitStartingWidgets();	

	FTimerHandle StartGameTimer;
	GetWorld()->GetTimerManager().SetTimer(StartGameTimer, this, &APMGameModeBase::StartGame, 1.f, false);

	GetWorld()->GetTimerManager().SetTimer(StartMovementTimer, this, &APMGameModeBase::StartAllMovement, 3.f, false);

	CherryCoinDel.BindUFunction(this, FName(TEXT("SpawnSpecialCoin")), CherryCoinClass);
	GetWorld()->GetTimerManager().SetTimer(CherryCoinTimer, CherryCoinDel, 10.f, false);

}
``` 
</details>

# Splines ([code](Source/PacMan/Gameplay/Splines)) 

<details>
<summary>More</summary>
Splines are an important part of the design. They are used for movement (described in the next section) and for the spawning of coins.</br>
All the splines on which the player and the ghosts can move are placed on the level.
	
<img src="https://github.com/user-attachments/assets/120861a8-57ea-4334-a608-705612820c89" width="700">

</br>Each spline must be connected to its neighboring splines.	

<img src="https://github.com/user-attachments/assets/78fedaae-8e6a-4476-8ca2-b5c4b2361864" width="700">

 </br>Due to the fact that there is often a problem with losing references to other actors, I needed to create a simple script that will automatically connect all the splines.

<img src="https://github.com/user-attachments/assets/187602fd-d55d-4cd1-b21f-a8bab3666ff4" width="700">


<img src="https://github.com/user-attachments/assets/b654f8da-fcb1-412d-a4b3-1d635bc51aad" width="700">

</br>Splines are also used to fill levels with coins. To do this, the SpawnCoins function calculates the number of coins depending on the length of the spline and spawns the actors at the correct distance. 

```c++
void APMSpline::SpawnCoins()
{
	if (this->ActorHasTag(FName(TEXT("withoutCoins"))))
	{
		return;
	}

	if (!IsValid(SplineComponent))
	{
		return;
	}
	const int32 CoinsNumber = FMath::RoundToInt(SplineComponent->GetSplineLength() / CoinDistanceOnSpline);

	for (int32 i = 0; i <= CoinsNumber; ++i)
	{		
		const FVector& Location = SplineComponent->GetLocationAtDistanceAlongSpline(CoinDistanceOnSpline * i, ESplineCoordinateSpace::World);
		const FRotator& Rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

		switch (UPMGameInstance::GetCurrentLevelType())
		{
			case ELevelType::CLASSIC:
			{
				if (IsValid(ClassicCoinClass))
				{
					GetWorld()->SpawnActor<APMCoin>(ClassicCoinClass, Location, Rotation, SpawnInfo);
				}
				break;
			}
			case ELevelType::MAZE:
			{
				if (IsValid(MazeCoinClass))
				{
					GetWorld()->SpawnActor<APMCoin>(MazeCoinClass, Location, Rotation, SpawnInfo);
				}
				break;
			}
			default: break;
		}
	}
}
```

</details>

# Movement ([code player](Source/PacMan/Gameplay/Player/PMPlayer.cpp))([code ghost](Source/PacMan/Gameplay/Ghosts/PMGhost.cpp))

<details>
<summary>More</summary>
Movement is a key part of both modes, and also the most complex part of the project. At first I tried the traditional movement and use of collision, but soon realized that I needed something more precise. I decided to use splines for this. In short, the player character and ghosts move along splines. 

```c++
void APMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(CurrentSpline))
	{
		return;
	}

	if (bIsMoving)
	{
		PositionOnSpline += DeltaTime * MovingDirection * Speed;

		const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
	}

	if (CheckIfAtSplinePoint())
	{
		bIsMoving = false;
		UnmarkSpline();		
		ChooseNewSpline();
		MarkSpline();
	}
}
```

 ```c++
void APMGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		PositionOnSpline += DeltaTime * MovingDirection * Speed;
	
		const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);

		if (CheckIfAtPoint())
		{
			bIsMoving = false;
			HandleMovement();
		}
	}
}
```

The challenge was to change directions. With the system of splines and their connections to each other, it was simply enough to change the CurrentSpline along which the character moves. The implementation is different in Ghosts and Player Character. Movement in Ghosts is closely related to their AI, so all the details are in the Ghosts section below. 
</br>In the case of the Player character, the choice of spline depends on the input of the player.
</br></br>The player's input is queued and stored in DesiredDirection. When the end of CurrentSpline is reached, a new spline is set depending on the saved DesiredDirection. The character is rotated and placed on the next spline.  

 ```c++
void APMPlayer::ChooseNewSpline()
{
	APMSpline* NewSpline = nullptr;

	switch (DesiredDirection)
	{
		case EPlayerDirection::UPWARD:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
			break;
		}		
		case EPlayerDirection::DOWN:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].DOWN;
			break;
		}			
		case EPlayerDirection::RIGHT:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
			break;
		}			
		case EPlayerDirection::LEFT:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].LEFT;
			break;
		}		
		default: break;
	}

	// If there is a spline leading to the Ghost base on the desired direction, ignore it, save the desired direction
	// and choose the spline leading forward on the next tick. 
	if (!NewSpline || NewSpline->ActorHasTag(FName(TEXT("releaseGhost"))))
	{
		TempDirection = DesiredDirection;
		DesiredDirection = CurrentDirection;
		return;
	}

	float NewYaw = 0.f;
	if (DesiredDirection == EPlayerDirection::UPWARD)
	{
		NewYaw = -90.f;
	}
	else if (DesiredDirection == EPlayerDirection::DOWN)
	{
		NewYaw = 90.f;
	}	
	else if (DesiredDirection == EPlayerDirection::LEFT)
	{
		NewYaw = 180.f;
	}
	
	SetActorRotation(FRotator(0, NewYaw, 0));
	CurrentSpline = NewSpline;
	PositionOnSpline = (DesiredDirection == EPlayerDirection::UPWARD || DesiredDirection == EPlayerDirection::RIGHT) ? 1.f :
			     CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
	CurrentDirection = DesiredDirection;
	MovingDirection = (DesiredDirection == EPlayerDirection::UPWARD || DesiredDirection == EPlayerDirection::RIGHT) ? 1.f : -1.f;
	bIsMoving = true;

	if (TempDirection != EPlayerDirection::NONE)
	{
		DesiredDirection = TempDirection;
	}
}
```
</br>In classic mode, the input is in 4 directions. Up, left, right, down. By pressing “W” or “up arrow” we queue that direction or do nothing if the character is already moving in that direction. We also check if the current direction is opposite to the upward movement. If so, the character stays in the same spline, but changes the direction of movement.

 ```c++
void APMClassicPlayer::MoveUp()
{
	if (CurrentDirection == EPlayerDirection::UPWARD) return;

	TempDirection = EPlayerDirection::NONE;
	DesiredDirection = EPlayerDirection::UPWARD;

	if (CurrentDirection == EPlayerDirection::DOWN)
	{
		Rotate180(EPlayerDirection::UPWARD);
	}
}
```

</br>In maze mode, there are only 3 inputs available: Left, Right and Backward (spacebar). Desired direction depends on the current direction and is set in clockwise (MoveRight) or counterclockwise (MoveLeft) order. Backward is simply turning around. 

 ```c++
void APMMazePlayer::MoveLeft()
{
	/**
	* Counterclockwise (e.g. if CurrentDirection is RIGHT, DesiredDirection will be UPWARD)
	* 
	*			 UPWARD
	*		    LEFT	RIGHT
	*			  DOWN
	*/
	switch (CurrentDirection)
	{
		case EPlayerDirection::UPWARD:
		{
			DesiredDirection = EPlayerDirection::LEFT;
			break;
		}
		case EPlayerDirection::DOWN:
		{
			DesiredDirection = EPlayerDirection::RIGHT;
			break;
		}
		case EPlayerDirection::LEFT:
		{
			DesiredDirection = EPlayerDirection::DOWN;
			break;
		}
		case EPlayerDirection::RIGHT:
		{			
			DesiredDirection = EPlayerDirection::UPWARD;
			break;
		}
		default: break;
	}

	TempDirection = EPlayerDirection::NONE;
}
```

</details>

# Ghosts ([code](Source/PacMan/Gameplay/Ghosts)) 

<details>
<summary>More</summary>
  
</details>

# Coins ([code](Source/PacMan/Gameplay/Coins)) 

<details>
<summary>More</summary>
Coins are divided into different types: basic, cherry, life, map. Each has a different use but share one common functionality, they can be collected by the player.</br></br>    
Inheritance and IMPMInteractionInterface allows all coin types to easily communicate with the player character.
  
```c++
class PACMAN_API APMCoin : public AActor, public IPMInteractionInterface
{
  ...
public:
	virtual int32 Interaction() override; 
```
</br>
In this case, in the PMPlayer class, the OnOverlapBegin function when interacting with each coin calls a function that adds potential points and at the same time calls a function Interaction()  which has a different implementation depending on the type of coin.

```c++
void APMPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (IPMInteractionInterface* InteractionInterface = Cast<IPMInteractionInterface>(OtherActor))
		{
			GameMode->AddPoints(InteractionInterface->Interaction());
		}
	}
}
```
```c++
int32 APMMapCoin::Interaction()
{
	if (APMGameModeMaze* GameMode = Cast<APMGameModeMaze>(UGameplayStatics::GetGameMode(GetWorld())))
	{	
		// Interact only when the player does not have all the maps
		if (GameMode->GetMapsNumber() < 2)
		{			
			if (PickUpSound != nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
			}
			GameMode->AddMap();		
			Destroy();
		}
	}
	return 0;
}
```
```c++
int32 APMAttackCoin::Interaction()
{
	if (APMGameModeBase* GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->StartPlayerAttackState();
	}
	CoinsCounter--;
	Destroy();
	return 10;
}
```
</details>
