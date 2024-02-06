// Fill out your copyright notice in the Description page of Project Settings.


#include "PMGhost.h"
#include "Kismet/GameplayStatics.h"
#include "PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "PMPlayer.h"
#include "PMGameModeBase.h"
#include <Queue>


// Sets default values
APMGhost::APMGhost()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	MovingDirection = 1.f;
	PositionOnSpline = 0.f;
	bIsMoving = true;
	SplineIndex = 0;
	SetActorRotation(FRotator(0, 0, 0));
	State = EGhostState::PASSIVE;
}

// Called when the game starts or when spawned
void APMGhost::BeginPlay()
{
	Super::BeginPlay();
	
	if (PawnSensing != nullptr)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &APMGhost::OnSeePawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | PawnSensing is nullptr"));
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), GhostTag, OutActors);

	for (AActor* item : OutActors)
	{
		CurrentSpline = Cast<APMSpline>(item);
	}

	if (CurrentSpline == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | CurrentSpline is nullptr"));
		return;
	}

	Player = Cast<APMPlayer>(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPawn());
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | Player is nullptr"));
		return;
	}
	
	if (State == EGhostState::WAIT)
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &APMGhost::ReleaseGhost, ReleaseTime, false);
	}
}

// Called every frame
void APMGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSpline == nullptr) return;

	if (bIsMoving)
	{
		PositionOnSpline += DeltaTime * MovingDirection * Speed;
	}
	
	const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
	SetActorLocation(NewLocation);

	if (CheckIfAtPoint())
	{
		ChooseNewSpline();
	}

}

bool APMGhost::CheckIfAtPoint()
{
	if (GetActorLocation().Equals(CurrentSpline->SplineComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World), 0.5f))
	{
		SplineIndex = 1;
		return true;
	}
	else if (GetActorLocation().Equals(CurrentSpline->SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), 0.5f))
	{
		SplineIndex = 0;
		return true;
	}

	return	false;
}

void APMGhost::ChooseNewSpline()
{
	TArray<int32> validSplines;
	int32 counter = 0;

	if (CurrentSpline->Splines[SplineIndex].UPWARD != nullptr)
	{
		validSplines.Add(0);
		counter++;
	}		
	if (CurrentSpline->Splines[SplineIndex].DOWN != nullptr)
	{
		validSplines.Add(1);
		counter++;
	}
	if (CurrentSpline->Splines[SplineIndex].LEFT != nullptr)
	{
		validSplines.Add(2);
		counter++;
	}
	if (CurrentSpline->Splines[SplineIndex].RIGHT != nullptr)
	{
		validSplines.Add(3);
		counter++;
	}

	if (counter == 0)
	{
  		MovingDirection *= -1.f;
		return;
	}

	switch (State)
	{
		case EGhostState::PASSIVE:
		{
			int32 randomIndex = FMath::RandRange(0, counter - 1);

			switch (validSplines[randomIndex])
			{
				case 0:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
					return;
				}
				case 1:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 180, 0));
					return;
				}
				case 2:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, -90, 0));
					return;
				}
				case 3:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 90, 0));
					return;
				}
				default: return;
			}
		}
		case EGhostState::ATTACK:
		{
			int32 choosenSpline = FindPath();

			//UE_LOG(LogTemp, Warning, TEXT("sss: %s"), *FString::FromInt(choosenSpline));
			
			switch (choosenSpline)
			{
				case -1:
				{
					if (CurrentSpline->ActorHasTag(FName("markedSpline")))
					{
						//UE_LOG(LogTemp, Warning, TEXT("TEN SAM"));
					}
					return;
				}
				case 0:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
					//UE_LOG(LogTemp, Warning, TEXT("UPWARD"));
					return;
				}
				case 1:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 180, 0));
					//UE_LOG(LogTemp, Warning, TEXT("DOWN"));
					return;
				}
				case 2:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, -90, 0));
					//UE_LOG(LogTemp, Warning, TEXT("LEFT"));
					return;
				}
				case 3:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 90, 0));
					//UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
					return;
				}
			default: return;
			}
		}
		case EGhostState::WAIT:
		{
			if (MovingDirection == 1.f)
			{
				if (CurrentSpline->Splines[1].UPWARD != nullptr && !CurrentSpline->Splines[1].UPWARD->ActorHasTag(TEXT("releaseGhost")))
				{
					CurrentSpline = CurrentSpline->Splines[1].UPWARD;
					PositionOnSpline = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
				}
				else
				{
					MovingDirection = -1.f;
				}
			}
			else if (MovingDirection == -1.f)
			{
				if (CurrentSpline->Splines[0].DOWN != nullptr)
				{
					CurrentSpline = CurrentSpline->Splines[0].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					SetActorRotation(FRotator(0, 180, 0));
				}
				else
				{
					MovingDirection = 1.f;
				}
			}
		}
		default: return;
	}
}

int32 APMGhost::FindPath()
{
	TMap<FString, APMSpline*> visitedSplines;
	std::queue<FSplineQueueData> splineQueue;

	TMap<int32, APMSpline*> validSplines = AvailableSplines(CurrentSpline, SplineIndex);

	for (auto& item : validSplines)
	{
		if (item.Value->ActorHasTag(FName("markedSpline")))
		{
			return item.Key;
		}

		splineQueue.push(FSplineQueueData(item.Key, item.Key, item.Value));	
	}

	while (!splineQueue.empty())
	{
		APMSpline* checkedSpline = splineQueue.front().Spline;
		int32 firstSpline = splineQueue.front().firstSpline;
		int32 nextSplineIndex = splineQueue.front().currentSplineIndex;
		splineQueue.pop();

		if (nextSplineIndex == 0 || nextSplineIndex == 3)
		{
			validSplines = AvailableSplines(checkedSpline, 1);
		}
		else if (nextSplineIndex == 1 || nextSplineIndex == 2)
		{
			validSplines = AvailableSplines(checkedSpline, 0);
		}

		for (auto& item : validSplines)
		{
			FString splineName = item.Value->GetName();
			if (!visitedSplines.Contains(splineName))
			{
				visitedSplines.Add(splineName, item.Value);
				splineQueue.push(FSplineQueueData(firstSpline, item.Key, item.Value));

				if (item.Value->ActorHasTag(FName("markedSpline")))
				{
					return firstSpline;
				}
			}
		}		
	}

	return -1;
}

void APMGhost::OnSeePawn(APawn* OtherPawn)
{
	if (State == EGhostState::PASSIVE && OtherPawn == Player)
	{		
		State = EGhostState::ATTACK;
		Player->MarkSpline();		
		GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &APMGhost::AttackTimer, 1.0f, true); //start timer
	}
	else if (State == EGhostState::ATTACK && OtherPawn == Player)
	{	
		ChaseTimeCounter = 0; //reset timer
	}
}

void APMGhost::AttackTimer()
{
	ChaseTimeCounter++;
	//UE_LOG(LogTemp, Warning, TEXT("Counter %s"), AttackTimeCounter);
	if (ChaseTimeCounter > MaxChaseTime)
	{
		ChaseTimeCounter = 0;
		if (ChaseTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ChaseTimerHandle);
		}		
		State = EGhostState::PASSIVE;
	}
}

int32 APMGhost::Interaction()
{
	APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (gameMode != nullptr)
	{
		gameMode->HandleGhostHit();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGhost::Interaction | GameMode is nullptr"));
	}
	
	return 0;
}

void APMGhost::ReleaseGhost()
{
	Player->MarkSpline();
	State = EGhostState::ATTACK;
	GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &APMGhost::AttackTimer, 1.0f, true);
}

void APMGhost::ResetGhost()
{
	bIsMoving = false;
	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMGhost::ResetStartingSpline, 2.f, false);
}

void APMGhost::ResetStartingSpline()
{
	this->SetActorHiddenInGame(true);

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), GhostTag, OutActors);

	for (AActor* item : OutActors)
	{
		CurrentSpline = Cast<APMSpline>(item);
	}

	if (CurrentSpline != nullptr)
	{						
		PositionOnSpline = 1.f;
		MovingDirection = 1.f;		
		UE_LOG(LogTemp, Warning, TEXT("ghost znika"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGhost::ResetStartingSpline | CurrentSpline is nullptr"));
	}
}

void APMGhost::StartGhost()
{
	this->SetActorHiddenInGame(false);
	bIsMoving = true;
}

TMap<int32, APMSpline*> APMGhost::AvailableSplines(APMSpline* Spline, int32 index)
{
	TMap<int32, APMSpline*> validSplines;

	if (Spline->Splines[index].UPWARD != nullptr)
	{
		validSplines.Add(0, Spline->Splines[index].UPWARD);
	}
	if (Spline->Splines[index].DOWN != nullptr)
	{
		validSplines.Add(1, Spline->Splines[index].DOWN);
	}
	if (Spline->Splines[index].LEFT != nullptr)
	{
		validSplines.Add(2, Spline->Splines[index].LEFT);
	}
	if (Spline->Splines[index].RIGHT != nullptr)
	{
		validSplines.Add(3, Spline->Splines[index].RIGHT);
	}

	return validSplines;
}




