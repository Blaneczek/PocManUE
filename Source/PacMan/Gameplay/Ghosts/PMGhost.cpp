// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGhost.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Gameplay/Player/PMPlayer.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <Queue>
#include "Sound/SoundWave.h"
#include "GameInstance/PMGameInstance.h"

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
	bCanSee = true;
	SplineIndex = 0;
	Speed = NormalSpeed;
	StartingMovingDirection = 1.f;
	StartingRotation = FRotator(0, 0, 0);
	SetEyesPosition(0);
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
  		MovingDirection *= -1;
		const float yawRotation = GetActorRotation().Yaw + (-180 * MovingDirection);
		SetActorRotation(FRotator(0, yawRotation, 0));
		SetEyesPosition(yawRotation);
		bIsMoving = true;
		return;
	}

	// 0 - UPWARD, 1 - DOWN, 2 - LEFT, 3 - RIGHT
	switch (State)
	{
		case EGhostState::PASSIVE:
		{
			const int32 randomIndex = FMath::RandRange(0, counter - 1);

			switch (validSplines[randomIndex])
			{
				case 0:
				{
					if (CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost"))) return;

					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, -90, 0));
					SetEyesPosition(-90);
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
					SetEyesPosition(90);
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
					SetEyesPosition(180);
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
					SetEyesPosition(0);
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
					const float yawRotation = GetActorRotation().Yaw + (-180 * MovingDirection);
					SetActorRotation(FRotator(0, yawRotation, 0));
					PositionOnSpline += MovingDirection;
					SetEyesPosition(yawRotation);
					bIsMoving = true;
					return;
				}
				case 0:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, -90, 0));
					SetEyesPosition(-90);
					bIsMoving = true;
					return;
				}
				case 1:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 90, 0));
					SetEyesPosition(90);
					bIsMoving = true;
					return;
				}
				case 2:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
					PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
					MovingDirection = -1.f;
					SetActorRotation(FRotator(0, 180, 0));
					SetEyesPosition(180);
					bIsMoving = true;
					return;
				}
				case 3:
				{
					CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
					PositionOnSpline = 1.f;
					MovingDirection = 1.f;
					SetActorRotation(FRotator(0, 0, 0));
					SetEyesPosition(0);
					bIsMoving = true;
					return;
				}
			default: return;
			}
		}
		case EGhostState::WAIT:
		{
			if (CurrentSpline->Splines[SplineIndex].UPWARD != nullptr && CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(TEXT("releaseGhost")))
			{
				MovingDirection *= -1;
				PositionOnSpline += 5 * MovingDirection;
				const float yawRotation = GetActorRotation().Yaw + (-180 * MovingDirection);
				SetActorRotation(FRotator(0, yawRotation, 0));
				SetEyesPosition(yawRotation);
				UE_LOG(LogTemp, Warning, TEXT("%f"), yawRotation);
				bIsMoving = true;
				return;
			}

			if (CurrentSpline->Splines[SplineIndex].UPWARD != nullptr)
			{
				CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
				MovingDirection = 1;
				PositionOnSpline = 1.f;
				bIsMoving = true;
				return;
			}

			if (CurrentSpline->Splines[SplineIndex].DOWN != nullptr)
			{
				CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
				MovingDirection = -1;
				PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
				bIsMoving = true;
				return;
			}

			return;
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
	if (bVulnerable || bGhostHitted || !bCanSee)
	{
		return;
	}

	if (State == EGhostState::PASSIVE && OtherPawn == Player)
	{		
		State = EGhostState::ATTACK;

		if (APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			gameMode->SetPlayerChased(true);
		}

		Player->MarkSpline();		
		GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &APMGhost::AttackTimer, 1.0f, true); //start timer
	}
}

void APMGhost::AttackTimer()
{
	if (ChaseTimerHandle.IsValid() && bGhostHitted)
	{
		GetWorld()->GetTimerManager().ClearTimer(ChaseTimerHandle);
		return;
	}

	ChaseTimeCounter++;
	if (ChaseTimeCounter > MaxChaseTime)
	{
		ChaseTimeCounter = 0;
		if (ChaseTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ChaseTimerHandle);
		}	

		if (APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			if (State != EGhostState::RELEASE) gameMode->SetPlayerChased(false);
		}

		bCanSee = false;
		GetWorld()->GetTimerManager().SetTimer(CanSeeTimerHandle, this, &APMGhost::CanSee, 2.0f, false);
		State = EGhostState::PASSIVE;
	}
}

int32 APMGhost::Interaction()
{
	if (bDoOnce)
	{
		APMGameModeBase* gameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (gameMode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("APMGhost::Interaction | GameMode is nullptr"));
			return 0;
		}

		if (!bVulnerable)
		{	
			gameMode->HandleGhostHit();		
		}
		else
		{
			gameMode->StopAllMovement();
			BackToBase();
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

	Speed = bVulnerable ? VulnerableSpeed : NormalSpeed;
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
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMGhost::HideGhost, 1.f, false);
}

void APMGhost::HideGhost()
{
	this->SetActorHiddenInGame(true);
}

void APMGhost::StartGhost()
{	
	bDoOnce = true;
	bVulnerable = false;
	bGhostHitted = false;
	CurrentSpline = StartingSpline;
	MovingDirection = StartingMovingDirection;
	SetActorRotation(StartingRotation);
	SetEyesPosition(StartingRotation.Yaw);
	PositionOnSpline = 1.f;
	State = StartingState;	

	Speed = State == EGhostState::WAIT ? VulnerableSpeed : NormalSpeed;

	if (CurrentSpline != nullptr)
	{
		const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
	}

	this->SetActorHiddenInGame(false);
}

void APMGhost::StartMovement()
{
	bIsMoving = true;
	if (State == EGhostState::WAIT)
	{
		Speed = VulnerableSpeed;
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimerHandle, this, &APMGhost::ReleaseGhost, ReleaseTime, false);
	}
}

void APMGhost::StopMovement()
{
	bIsMoving = false;
}

void APMGhost::VulnerableState()
{
	if (bGhostHitted) return;

	if (VulnerableTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableTimerHandle);
	}

	bVulnerable = true;
	Speed = VulnerableSpeed;

	if (DynMaterial != nullptr)
	{
		DynMaterial->SetVectorParameterValue(TEXT("Color"), VulnerableColor);
	}
	
	if (State == EGhostState::ATTACK)
	{
		State = EGhostState::PASSIVE;
		MovingDirection *= -1.f;
		const float yawRotation = GetActorRotation().Yaw + (-180 * MovingDirection);
		SetActorRotation(FRotator(0, yawRotation, 0));
		SetEyesPosition(yawRotation);
	}

	GetWorld()->GetTimerManager().SetTimer(VulnerableTimerHandle, this, &APMGhost::EndVulnerableState, 7.f, false);
	GetWorld()->GetTimerManager().SetTimer(FlickeringTimerHandle, this, &APMGhost::VulnerableFlickering, 0.3f, true, 5);
}

void APMGhost::EndVulnerableState()
{
	bGhostHitted = false;
	bVulnerable = false;
	Speed = State == EGhostState::WAIT ? VulnerableSpeed : NormalSpeed;
	bDoOnce = true;
	bFlickering = false;

	if (APMGameModeBase* GM = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->EndPlayerAttackState();
	}
	
	if (VulnerableTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableTimerHandle);
	}
	if (FlickeringTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FlickeringTimerHandle);
	}

	if (DynMaterial != nullptr)
	{
		DynMaterial->SetVectorParameterValue(TEXT("Color"), StartingColor);
	}
}

void APMGhost::VulnerableFlickering()
{
	if (DynMaterial == nullptr)
	{
		return;
	}

	if (!bFlickering )
	{
		DynMaterial->SetVectorParameterValue(TEXT("Color"), VulnerableColor);
		bFlickering = true;
	}
	else
	{
		DynMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.f, 1.f, 1.f, 1));
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

	if (HitSoundClassic != nullptr) UGameplayStatics::PlaySound2D(this, HitSoundClassic);

	Speed = ReturnSpeed;
	bIsMoving = true;
	bGhostHitted = true;
	State = EGhostState::RELEASE;
}

void APMGhost::CanSee()
{
	bCanSee = true;
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




