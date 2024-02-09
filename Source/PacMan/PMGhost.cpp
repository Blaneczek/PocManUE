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
#include "Materials/MaterialInstanceDynamic.h"
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
	PositionOnSpline = 1.f;
	bIsMoving = false;
	bDoOnce = true;
	bFlickering = false;
	bGhostHitted = false;
	SplineIndex = 0;
	Speed = NormalSpeed;
	StartingState = EGhostState::PASSIVE;
	StartingMovingDirection = 1.f;
	StartingRotation = FRotator(0, 0, 0);
}

// Called when the game starts or when spawned
void APMGhost::BeginPlay()
{
	Super::BeginPlay();

	DynMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	DynMaterial->SetVectorParameterValue("Color", StartingColor);
	Mesh->SetMaterial(0, DynMaterial);

	if (PawnSensing != nullptr)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &APMGhost::OnSeePawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | PawnSensing is nullptr"));
	}

	if (StartingSpline != nullptr)
	{
		StartGhost();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | StartingSpline is nullptr"));
	}

	Player = Cast<APMPlayer>(UGameplayStatics::GetPlayerController(GetWorld(),0)->GetPawn());
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | Player is nullptr"));
		return;
	}

	OnGhostHitDelegate.AddUObject(this, &APMGhost::ResetGhost);
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
		bIsMoving = false;
		ChooseNewSpline();
	}
}

bool APMGhost::CheckIfAtPoint()
{
	if (PositionOnSpline >= CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1))
	{
		SplineIndex = 1;
		return true;
	}
	else if (PositionOnSpline <= CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(0))
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
		const float& yaw = GetActorRotation().Yaw;
		SetActorRotation(FRotator(0, yaw + 180, 0));
		bIsMoving = true;
		return;
	}

	// 0 - UPWARD, 1 - DOWN, 2 - LEFT, 3 - RIGHT
	switch (State)
	{
		case EGhostState::PASSIVE:
		{
			int32 randomIndex = FMath::RandRange(0, counter - 1);

			switch (validSplines[randomIndex])
			{
				case 0:
				{
					if (CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost"))) return;

					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, -90, 0));
					bIsMoving = true;
					return;
				}
				case 1:
				{
					if (CurrentSpline->Splines[SplineIndex].DOWN->ActorHasTag(FName("releaseGhost"))) return;

					CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 90, 0));
					bIsMoving = true;
					return;
				}
				case 2:
				{
					if (CurrentSpline->Splines[SplineIndex].LEFT->ActorHasTag(FName("releaseGhost"))) return;

					CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 180, 0));
					bIsMoving = true;
					return;
				}
				case 3:
				{
					if (CurrentSpline->Splines[SplineIndex].RIGHT->ActorHasTag(FName("releaseGhost"))) return;

					CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
					bIsMoving = true;
					return;
				}
				default: return;
			}
		}
		case EGhostState::RELEASE:
		case EGhostState::ATTACK: 
		{
			int32 choosenSpline = FindPath();
			
			switch (choosenSpline)
			{
				case -1:
				{
					MovingDirection *= -1.f;
					const float yaw = GetActorRotation().Yaw;
					SetActorRotation(FRotator(0, yaw + 180, 0));
					bIsMoving = true;
					return;
				}
				case 0:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, -90, 0));
					bIsMoving = true;
					return;
				}
				case 1:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 90, 0));
					bIsMoving = true;
					return;
				}
				case 2:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 180, 0));
					bIsMoving = true;
					return;
				}
				case 3:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
					bIsMoving = true;
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
					SetActorRotation(FRotator(0, -90, 0));
					bIsMoving = true;
				}
				else
				{
					MovingDirection = -1.f;
					bIsMoving = true;
				}
			}
			else if (MovingDirection == -1.f)
			{
				if (CurrentSpline->Splines[0].DOWN != nullptr)
				{
					CurrentSpline = CurrentSpline->Splines[0].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					SetActorRotation(FRotator(0, 90, 0));
					bIsMoving = true;
				}
				else
				{
					MovingDirection = 1.f;
					bIsMoving = true;
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
		if (!bGhostHitted && item.Value->ActorHasTag(FName("markedSpline")))
		{
			return item.Key;
		}
		else if (bGhostHitted && item.Value->ActorHasTag(FName("base")))
		{	
			EndVulnerableState();
			ReleaseGhost();
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

				if ( (!bGhostHitted && item.Value->ActorHasTag(FName("markedSpline")))
					|| (bGhostHitted && item.Value->ActorHasTag(FName("base"))) )
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
	if (bVulnerable || bGhostHitted)
	{
		return;
	}

	if (State == EGhostState::PASSIVE && OtherPawn == Player)
	{		
		State = EGhostState::ATTACK;
		Player->MarkSpline();		
		GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &APMGhost::AttackTimer, 1.0f, true); //start timer
	}
}

void APMGhost::AttackTimer()
{
	ChaseTimeCounter++;
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
	if (bDoOnce)
	{
		APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (!bVulnerable)
		{			
			if (gameMode != nullptr)
			{
				gameMode->HandleGhostHit();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("APMGhost::Interaction | GameMode is nullptr"));
			}
		}
		else
		{
			if (gameMode != nullptr)
			{
				gameMode->StopAllMovement();
				BackToBase();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("APMGhost::Interaction | GameMode is nullptr"));
			}

			bDoOnce = false;
			return 200;
		}	

		bDoOnce = false;
	}	
	return 0;
}

void APMGhost::ReleaseGhost()
{
	if (ReleaseTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
	}

	Player->MarkSpline();
	State = EGhostState::RELEASE;
	GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &APMGhost::AttackTimer, 1.0f, true);
}

void APMGhost::ResetGhost()
{
	if (ReleaseTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReleaseTimerHandle);
	}
	if (ChaseTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ChaseTimerHandle);
	}
	bIsMoving = false;
	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMGhost::HideGhost, 2.f, false);
}

void APMGhost::HideGhost()
{
	this->SetActorHiddenInGame(true);
}

void APMGhost::StartGhost()
{	
	bDoOnce = true;
	Speed = NormalSpeed;
	bVulnerable = false;
	bGhostHitted = false;
	CurrentSpline = StartingSpline;
	MovingDirection = StartingMovingDirection;
	SetActorRotation(StartingRotation);
	PositionOnSpline = 1.f;
	State = StartingState;	

	if (CurrentSpline != nullptr)
	{
		const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
	}

	if (State == EGhostState::WAIT)
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &APMGhost::ReleaseGhost, ReleaseTime, false);
	}

	FTimerHandle StartMovementTimer;
	GetWorld()->GetTimerManager().SetTimer(StartMovementTimer, this, &APMGhost::StartMovement, 1, false);

	this->SetActorHiddenInGame(false);
}

void APMGhost::StartMovement()
{
	bIsMoving = true;
}

void APMGhost::StopMovement()
{
	bIsMoving = false;
}

void APMGhost::VulnerableState()
{
	if (bGhostHitted)
	{
		return;
	}

	if (VulnerableTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableTimerHandle);
	}
	bVulnerable = true;
	Speed = VulnerableSpeed;
	if (DynMaterial != nullptr)
	{
		DynMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.f, 0.f, 1.f, 1));
	}
	
	if (State == EGhostState::ATTACK)
	{
		State = EGhostState::PASSIVE;
	}

	GetWorld()->GetTimerManager().SetTimer(VulnerableTimerHandle, this, &APMGhost::EndVulnerableState, 7.f, false);
	GetWorld()->GetTimerManager().SetTimer(FlickeringTimerHandle, this, &APMGhost::VulnerableFlickering, 0.3f, true, 5);
}

void APMGhost::EndVulnerableState()
{
	if (VulnerableTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableTimerHandle);
	}
	if (FlickeringTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FlickeringTimerHandle);
	}
	bGhostHitted = false;
	bVulnerable = false;
	Speed = NormalSpeed;
	bDoOnce = true;
	bFlickering = false;

	if (DynMaterial != nullptr)
	{
		DynMaterial->SetVectorParameterValue(TEXT("Color"), StartingColor);
	}
}

void APMGhost::VulnerableFlickering()
{
	if (!bFlickering )
	{
		if (DynMaterial != nullptr)
		{
			DynMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.f, 0.f, 1.f, 1));
		}

		bFlickering = true;
	}
	else
	{
		if (DynMaterial != nullptr)
		{
			DynMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 1.f, 1.f, 1));
		}

		bFlickering = false;
	}
}

void APMGhost::BackToBase()
{
	if (VulnerableTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableTimerHandle);
	}
	if (FlickeringTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FlickeringTimerHandle);
	}
	Speed = ReturnSpeed;
	bIsMoving = true;
	bGhostHitted = true;
	State = EGhostState::ATTACK;
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




