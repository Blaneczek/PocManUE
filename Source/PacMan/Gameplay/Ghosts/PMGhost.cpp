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

	State = EGhostState::NONE;
	CurrentDirection = EGhostDirection::NONE;
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
	ChaseTimeCounter = 0;
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

	GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnStartGame.AddUObject(this, &APMGhost::StartGhost);
		GameMode->OnStopGame.AddUObject(this, &APMGhost::ResetGhost);
		GameMode->OnStartMovement.AddUObject(this, &APMGhost::StartMovement);
		GameMode->OnStopMovement.AddUObject(this, &APMGhost::StopMovement);
		GameMode->OnPlayerAttack.AddUObject(this, &APMGhost::VulnerableState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGhost::BeginPlay | GameMode is nullptr"));
	}

	if (PawnSensing != nullptr)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &APMGhost::OnSeePawn);
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
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("PMGhost::BeginPlay | Player is nullptr"));
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
		HandleMovement();
	}
}

TArray<int32> APMGhost::FindValidSplinesInRandomMovement()
{
	TArray<int32> OutValidSplines;

	if ((CurrentSpline->Splines[SplineIndex].UPWARD != nullptr) && (!CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost"))))
	{
		OutValidSplines.Add(0);
	}
	if ((CurrentSpline->Splines[SplineIndex].DOWN != nullptr) && (!CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost"))))
	{
		OutValidSplines.Add(1);
	}
	if ((CurrentSpline->Splines[SplineIndex].LEFT != nullptr) && (!CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost"))))
	{
		OutValidSplines.Add(2);
	}
	if ((CurrentSpline->Splines[SplineIndex].RIGHT != nullptr) && (!CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost"))))
	{
		OutValidSplines.Add(3);
	}

	return OutValidSplines;
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

void APMGhost::HandleMovement()
{
	if (State == EGhostState::BASE)
	{
		GhostBaseMovement();
		return;
	}

	if ((State == EGhostState::PASSIVE) || (State == EGhostState::VULNERABLE))
	{
		TArray<int32> ValidSplines = FindValidSplinesInRandomMovement();
		const int32 ValidSplinesNum = ValidSplines.Num();
		if (ValidSplinesNum == 0)
		{
			TurnAround();
			return;
		}

		const int32 RandomIndex = FMath::RandRange(-1, ValidSplinesNum - 1);
		ChooseNewSpline(ValidSplines[RandomIndex]);
		return;
	}

	if (State == EGhostState::ATTACK)
	{
		int32 FoundSpline = FindPath(FName("player_MarkedSpline")); 
		ChooseNewSpline(FoundSpline);
		return;
	}

	if (State == EGhostState::RELEASE)
	{
		int32 FoundSpline = FindPath(FName("release_MarkedSpline"));
		ChooseNewSpline(FoundSpline);
		return;
	}

	if (State == EGhostState::HITTED)
	{
		int32 FoundSpline = FindPath(FName("base_MarkedSpline"));
		ChooseNewSpline(FoundSpline);
		return;
	}


	/*
	if (ValidSplinesNum == 0)
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
		case EGhostState::VULNERABLE:
		case EGhostState::PASSIVE:
		{
			const int32 randomIndex = FMath::RandRange(0, ValidSplinesNum - 1);

			switch (ValidSplines[randomIndex])
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
		case EGhostState::HITTED:
		case EGhostState::RELEASE:
		case EGhostState::ATTACK: 
		{
			int32 choosenSpline = FindPath(FName("sds")); //TODO: marked spline as parameter
			
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
	}*/

}

int32 APMGhost::FindPath(const FName& SplineTag)
{
	TMap<FString, APMSpline*> VisitedSplines;
	TQueue<FSplineQueueData> SplineQueue;

	TMap<int32, APMSpline*> ValidSplines = FindValidSplinesInMarkedMovement(CurrentSpline, SplineIndex);

	for (auto& item : ValidSplines)
	{
		if (item.Value->ActorHasTag(SplineTag))
		{
			ReachingMarkedSpline();
			return item.Key;			
		}
		SplineQueue.Enqueue(FSplineQueueData(item.Key, item.Key, item.Value));	
	}

	while (!SplineQueue.IsEmpty())
	{
		APMSpline* CheckedSpline = SplineQueue.Peek()->Spline;
		const int32 FirstSpline = SplineQueue.Peek()->FirstSplineIndex;
		const int32 NextSplineIndex = SplineQueue.Peek()->CurrentSplineIndex;
		SplineQueue.Pop();

		if (NextSplineIndex == 0 || NextSplineIndex == 3)
		{
			ValidSplines = FindValidSplinesInMarkedMovement(CheckedSpline, 1);
		}
		else if (NextSplineIndex == 1 || NextSplineIndex == 2)
		{
			ValidSplines = FindValidSplinesInMarkedMovement(CheckedSpline, 0);
		}

		for (auto& item : ValidSplines)
		{
			FString SplineName = item.Value->GetName();
			if (!VisitedSplines.Contains(SplineName))
			{
				VisitedSplines.Add(SplineName, item.Value);
				SplineQueue.Enqueue(FSplineQueueData(FirstSpline, item.Key, item.Value));

				if ( item.Value->ActorHasTag(SplineTag))
				{
					return FirstSpline;
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

		if (GameMode != nullptr)
		{
			GameMode->SetPlayerChased(true);
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

		if (GameMode != nullptr && State != EGhostState::RELEASE)
		{
			GameMode->SetPlayerChased(false);
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

	Speed = State == EGhostState::BASE ? VulnerableSpeed : NormalSpeed;

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
	if (State == EGhostState::BASE)
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
	Speed = State == EGhostState::BASE ? VulnerableSpeed : NormalSpeed;
	bDoOnce = true;
	bFlickering = false;
	
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

void APMGhost::MoveToNewSpline(APMSpline* NewSpline, float Direction, float YawRotation)
{
	if (!NewSpline)
	{
		CurrentSpline = NewSpline;
	}
	MovingDirection = Direction;
	PositionOnSpline = MovingDirection > 0 ? 1.f : CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
	SetActorRotation(FRotator(0, YawRotation, 0));
	SetEyesPosition(YawRotation);
	bIsMoving = true;
}

void APMGhost::TurnAround()
{
	const float NewMovingDirection = MovingDirection * -1.f;
	const float YawRotation = GetActorRotation().Yaw + (-180 * NewMovingDirection);
	MoveToNewSpline(nullptr, NewMovingDirection, YawRotation);
}

void APMGhost::GhostBaseMovement()
{
	if ((CurrentSpline->Splines[SplineIndex].UPWARD != nullptr) && (CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(TEXT("releaseGhost"))))
	{
		TurnAround();
		return;
	}

	if (CurrentSpline->Splines[SplineIndex].UPWARD != nullptr)
	{
		MoveToNewSpline(CurrentSpline->Splines[SplineIndex].UPWARD, 1.f, -90.f);
		return;
	}

	if (CurrentSpline->Splines[SplineIndex].DOWN != nullptr)
	{
		MoveToNewSpline(CurrentSpline->Splines[SplineIndex].DOWN, -1.f, 90.f);
		return;
	}

	return;
}

void APMGhost::ChooseNewSpline(int32 ChoosenSpline)
{
	switch (ChoosenSpline)
	{
		case -1:
		{
			TurnAround();
			return;
		}
		case 0:
		{
			MoveToNewSpline(CurrentSpline->Splines[SplineIndex].UPWARD, 1.f, -90.f);
			return;
		}
		case 1:
		{
			MoveToNewSpline(CurrentSpline->Splines[SplineIndex].DOWN, -1.f, 90.f);
			return;
		}
		case 2:
		{
			MoveToNewSpline(CurrentSpline->Splines[SplineIndex].LEFT, -1.f, 180.f);
			return;
		}
		case 3:
		{
			MoveToNewSpline(CurrentSpline->Splines[SplineIndex].RIGHT, 1.f, 0.f);
			return;
		}
		default: return;
	}
}

void APMGhost::ReachingMarkedSpline()
{
}

TMap<int32, APMSpline*> APMGhost::FindValidSplinesInMarkedMovement(APMSpline* Spline, int32 index)
{
	TMap<int32, APMSpline*> ValidSplines;

	if (Spline->Splines[index].UPWARD != nullptr)
	{
		ValidSplines.Add(0, Spline->Splines[index].UPWARD);
	}
	if (Spline->Splines[index].DOWN != nullptr)
	{
		ValidSplines.Add(1, Spline->Splines[index].DOWN);
	}
	if (Spline->Splines[index].LEFT != nullptr)
	{
		ValidSplines.Add(2, Spline->Splines[index].LEFT);
	}
	if (Spline->Splines[index].RIGHT != nullptr)
	{
		ValidSplines.Add(3, Spline->Splines[index].RIGHT);
	}

	return ValidSplines;
}




