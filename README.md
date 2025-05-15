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
| [Game modes](#game-modes-code)                                                 | Managing the flow of gameplay in classic and maze mode          |
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


</details>

# Splines ([code](Source/PacMan/Gameplay/Splines)) 

<details>
<summary>More</summary>
  
</details>

# Movement ([code player](Source/PacMan/Gameplay/Player/PMPlayer.cpp))([code ghost](Source/PacMan/Gameplay/Ghosts/PMGhost.cpp))

<details>
<summary>More</summary>
  
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
