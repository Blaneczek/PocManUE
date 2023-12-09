// Fill out your copyright notice in the Description page of Project Settings.


#include "PMGhost.h"
#include "Kismet/GameplayStatics.h"
#include "PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "PMPlayer.h"
#include <Queue>


// Sets default values
APMGhost::APMGhost()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	MovingDirection = 1.f;
	PositionOnSpline = 0.f;
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
		UE_LOG(LogTemp, Warning, TEXT("Jest"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("nie ma"));
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), FName(TEXT("startGhost")), OutActors);

	for (AActor* item : OutActors)
	{
		CurrentSpline = Cast<APMSpline>(item);
	}

	if (CurrentSpline == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid CurrentSpline"));
		return;
	}

	Player = Cast<APMPlayer>(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPawn());
}

// Called every frame
void APMGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSpline == nullptr) return;

	PositionOnSpline += DeltaTime * MovingDirection * Speed;

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

			UE_LOG(LogTemp, Warning, TEXT("sss: %s"), *FString::FromInt(choosenSpline));
			
			switch (choosenSpline)
			{
				case -1:
				{
					if (CurrentSpline->ActorHasTag(FName("markedSpline")))
					{
						UE_LOG(LogTemp, Warning, TEXT("TEN SAM"));
					}
					return;
				}
				case 0:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
					UE_LOG(LogTemp, Warning, TEXT("UPWARD"));
					return;
				}
				case 1:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 180, 0));
					UE_LOG(LogTemp, Warning, TEXT("DOWN"));
					return;
				}
				case 2:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, -90, 0));
					UE_LOG(LogTemp, Warning, TEXT("LEFT"));
					return;
				}
				case 3:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 90, 0));
					UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
					return;
				}
			default: return;
			}
		}
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
		UE_LOG(LogTemp, Warning, TEXT("Saw actor"));
		State = EGhostState::ATTACK;
		Player->MarkSpline();
	}
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




