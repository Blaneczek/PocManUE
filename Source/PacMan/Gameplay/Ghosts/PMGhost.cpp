// Copyright (c) 2024 Dawid Szoldra. All rights reserved.


#include "PMGhost.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundWave.h"
#include "GameInstance/PMGameInstance.h"
#include "Components/WidgetComponent.h"

// Sets default values
APMGhost::APMGhost()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);

	PointsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PointWidget"));	
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	State = EGhostState::NONE;
	MovementState = EGhostMovementState::NONE;
	MovingDirection = 1.f;
	PositionOnSpline = 1.f;
	bIsMoving = false;
	bDoOnce = true;
	bFlickering = false;
	bCanSee = true;
	bTurnedAround = false;
	SplineIndex = 0;
	Speed = NormalSpeed;
	StartingMovingDirection = 1.f;
	MaxChaseTime = 4.f;
	StartingRotation = FRotator(0, 0, 0);
	SetEyesPosition(0);
}

// Called when the game starts or when spawned
void APMGhost::BeginPlay()
{
	Super::BeginPlay();

	InitializeMaterial();
	BindGameModeDelegates();

	PointsWidget->SetVisibility(false);

	if (PawnSensing != nullptr)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &APMGhost::OnSeePawn);
	}

}

// Called every frame
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

TArray<int32> APMGhost::FindValidSplinesInRandomMovement()
{
	TArray<int32> OutValidSplines;

	if ((CurrentSpline->Splines[SplineIndex].UPWARD != nullptr) && (!CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName(TEXT("releaseGhost")))))
	{
		OutValidSplines.Add(0);
	}
	if ((CurrentSpline->Splines[SplineIndex].DOWN != nullptr) && (!CurrentSpline->Splines[SplineIndex].DOWN->ActorHasTag(FName(TEXT("releaseGhost")))))
	{
		OutValidSplines.Add(1);
	}
	if ((CurrentSpline->Splines[SplineIndex].LEFT != nullptr) && (!CurrentSpline->Splines[SplineIndex].LEFT->ActorHasTag(FName(TEXT("releaseGhost")))))
	{
		OutValidSplines.Add(2);
	}
	if ((CurrentSpline->Splines[SplineIndex].RIGHT != nullptr) && (!CurrentSpline->Splines[SplineIndex].RIGHT->ActorHasTag(FName(TEXT("releaseGhost")))))
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
	int32 FoundSpline = 0;

	switch (MovementState)
	{
		case EGhostMovementState::BASE:
		{
			GhostBaseMovement();
			return;
		}
		case EGhostMovementState::PASSIVE:
		{
			TArray<int32> ValidSplines = FindValidSplinesInRandomMovement();
			const int32 ValidSplinesNum = ValidSplines.Num();
			if (ValidSplinesNum == 0)
			{
				TurnAround();
				return;
			}

			const int32 RandomIndex = FMath::RandRange(0, ValidSplinesNum - 1);
			const int32 RandomNum = FMath::RandRange(1, 10);

			//10% chance of turning around if not turned before 
			if (!bTurnedAround && RandomNum > 9)
			{
				FoundSpline = -1;
				bTurnedAround = true;
			}
			else
			{
				FoundSpline = ValidSplines[RandomIndex];
				bTurnedAround = false;
			}

			FoundSpline = RandomNum < 10 ? ValidSplines[RandomIndex] : -1; 
			break;
		}
		case EGhostMovementState::ATTACK:
		{
			FoundSpline = FindPath(FName(TEXT("player_MarkedSpline"))); 
			break;
		}
		case EGhostMovementState::RELEASE:
		{
			FoundSpline = FindPath(FName(TEXT("release_MarkedSpline")));
			break;
		}
		case EGhostMovementState::HITTED:
		{
			FoundSpline = FindPath(FName(TEXT("base_MarkedSpline")));
			break;
		}
		default: return;
	}

	ChooseNewSpline(FoundSpline);
}

// Implementation of the BFS (Breadth-First Search) algorithm to find a path to a target spline.
int32 APMGhost::FindPath(const FName& SplineTag)
{
	TMap<FString, APMSpline*> VisitedSplines;

	// Queue to store splines to be visited next and index of first spline from which the path begins (FSplineQueueData.FirstSplineIndex).
	TQueue<FSplineQueueData> SplineQueue;

	// Get all the splines from where the paths start 
	TMap<int32, APMSpline*> ValidSplines = FindValidSplinesInMarkedMovement(CurrentSpline, SplineIndex);

	for (auto& item : ValidSplines)
	{
		// Check if the ghost is already on the target spline.
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

		// 0 - UPWARD, 1 - DOWN, 2 - LEFT, 3 - RIGHT
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
					// Next spline the ghost should enter to reach the target spline 
					return FirstSpline;
				}
			}
		}		
	}

	// If the path doesn't exist, turn around
	return -1;
}

void APMGhost::OnSeePawn(APawn* OtherPawn)
{
	if (OtherPawn->ActorHasTag(FName(TEXT("player"))) && bCanSee && (MovementState == EGhostMovementState::PASSIVE) && (State != EGhostState::VULNERABLE))
	{		
		MovementState = EGhostMovementState::ATTACK;

		if (GameMode != nullptr)
		{
			GameMode->SetPlayerChased(true);
		}
	
		GetWorld()->GetTimerManager().SetTimer(ChaseTimer, this, &APMGhost::AttackTimer, MaxChaseTime, false); 
	}
}

void APMGhost::AttackTimer()
{
	bCanSee = false;
	FTimerDelegate CanSeeDel;
	CanSeeDel.BindLambda([&]() { bCanSee = true; });
	GetWorld()->GetTimerManager().SetTimer(CanSeeTimer, CanSeeDel, 2.f, false);
	MovementState = EGhostMovementState::PASSIVE;

	if (GameMode != nullptr)
	{
		GameMode->SetPlayerChased(false);
	}
}

int32 APMGhost::Interaction()
{
	if (GameMode == nullptr)
	{
		return 0;
	}

	if (bDoOnce)
	{		
		if (State == EGhostState::VULNERABLE)
		{	
			// Show the points widget where the ghost was hit
			PointsWidget->SetWorldLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 100));
			PointsWidget->SetVisibility(true);
			GameMode->StopAllMovement();
			BackToBase();
			bDoOnce = false;	
			return 200;
		}
		else if (MovementState == EGhostMovementState::ATTACK || MovementState == EGhostMovementState::PASSIVE)
		{
			GameMode->HandleGhostHit();
			bDoOnce = false;	
		}
	}	
	return 0;
}

void APMGhost::Release()
{
	bDoOnce = true;
	Speed = State == EGhostState::VULNERABLE ? VulnerableSpeed : NormalSpeed;
	MovementState = EGhostMovementState::RELEASE;
}

void APMGhost::Reset()
{
	State = EGhostState::NONE;
	MovementState = EGhostMovementState::NONE;

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	bIsMoving = false;
	FTimerHandle ResetTimer;
	FTimerDelegate ResetDel;
	ResetDel.BindUObject(this, &APMGhost::SetActorHiddenInGame, true);
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, ResetDel, 1.f, false);
}

void APMGhost::Start()
{	
	DynMaterial->SetVectorParameterValue(FName(TEXT("Color")), StartingColor);
	CurrentSpline = StartingSpline;
	MovingDirection = StartingMovingDirection;
	SetActorRotation(StartingRotation);
	SetEyesPosition(StartingRotation.Yaw);
	PositionOnSpline = 1.f;
	MovementState = StartingMovementState;	
	State = EGhostState::NONE;

	Speed = MovementState == EGhostMovementState::BASE ? VulnerableSpeed : NormalSpeed;

	if (CurrentSpline != nullptr)
	{
		const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
	}

	bDoOnce = true;
	SetActorHiddenInGame(false);
}

void APMGhost::StartMovement()
{
	PointsWidget->SetVisibility(false);
	bIsMoving = true;

	if (!GetWorld()->GetTimerManager().IsTimerActive(ReleaseTimer) && MovementState == EGhostMovementState::BASE)
	{
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimer, this, &APMGhost::Release, ReleaseTime, false);
	}
}

void APMGhost::StopMovement()
{
	bIsMoving = false;
}

void APMGhost::VulnerableState()
{
	if (MovementState == EGhostMovementState::HITTED)
	{
		return;
	}

	if (MovementState == EGhostMovementState::ATTACK)
	{
		TurnAround();
		MovementState = EGhostMovementState::PASSIVE;
	}

	DynMaterial->SetVectorParameterValue(FName(TEXT("Color")), VulnerableColor);
	Speed = VulnerableSpeed;
	State = EGhostState::VULNERABLE;

	GetWorld()->GetTimerManager().SetTimer(VulnerableTimer, this, &APMGhost::EndVulnerableState, 7.f, false);
	GetWorld()->GetTimerManager().SetTimer(FlickeringTimer, this, &APMGhost::VulnerableFlickering, 0.3f, true, 5);
}

void APMGhost::EndVulnerableState()
{
	State = EGhostState::NONE;
	Speed = MovementState == EGhostMovementState::BASE ? VulnerableSpeed : NormalSpeed;
	bFlickering = true;
	bDoOnce = true;

	if (VulnerableTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VulnerableTimer);
	}
	if (FlickeringTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FlickeringTimer);
	}

	DynMaterial->SetVectorParameterValue(FName(TEXT("Color")), StartingColor);
}

void APMGhost::VulnerableFlickering()
{
	if (State != EGhostState::VULNERABLE)
	{
		return;
	}

	if (!bFlickering )
	{
		DynMaterial->SetVectorParameterValue(FName(TEXT("Color")), VulnerableColor);
		bFlickering = true;
	}
	else
	{
		DynMaterial->SetVectorParameterValue(FName(TEXT("Color")), FLinearColor(1.f, 1.f, 1.f, 1));
		bFlickering = false;
	}
}

void APMGhost::BackToBase()
{
	if (HitSoundClassic != nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HitSoundClassic);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	MovementState = EGhostMovementState::HITTED;
	State = EGhostState::NONE;
	Speed = ReturnSpeed;
	bIsMoving = true;;
}

void APMGhost::CanSee()
{
	bCanSee = true;
}

void APMGhost::InitializeMaterial()
{
	if (Mesh != nullptr && Mesh->GetNumMaterials() > 0)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
		DynMaterial->SetVectorParameterValue(TEXT("Color"), StartingColor);
		Mesh->SetMaterial(0, DynMaterial);
	}	
}

void APMGhost::BindGameModeDelegates()
{
	GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnStartGame.AddUObject(this, &APMGhost::Start);
		GameMode->OnStopGame.AddUObject(this, &APMGhost::Reset);
		GameMode->OnStartMovement.AddUObject(this, &APMGhost::StartMovement);
		GameMode->OnStopMovement.AddUObject(this, &APMGhost::StopMovement);
		GameMode->OnPlayerAttack.AddUObject(this, &APMGhost::VulnerableState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMGhost::BindGameModeDelegates | GameMode is nullptr"));
	}
}

void APMGhost::MoveToNewSpline(float Direction, float YawRotation, APMSpline* NewSpline)
{
	MovingDirection = Direction;

	if (NewSpline)
	{
		CurrentSpline = NewSpline;
		PositionOnSpline = MovingDirection > 0 ? 1.f : CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
	}
	else
	{
		PositionOnSpline += MovingDirection > 0 ? 10.f : -10.f;
	}
		
	SetActorRotation(FRotator(0, YawRotation, 0));
	SetEyesPosition(YawRotation);
	bIsMoving = true;
}

void APMGhost::TurnAround()
{
	const float NewMovingDirection = (MovingDirection * -1.f);
	const float YawRotation = GetActorRotation().Yaw + (-180 * NewMovingDirection);
	MoveToNewSpline(NewMovingDirection, YawRotation);
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
		MoveToNewSpline(1.f, -90.f, CurrentSpline->Splines[SplineIndex].UPWARD);
		return;
	}

	if (CurrentSpline->Splines[SplineIndex].DOWN != nullptr)
	{
		MoveToNewSpline(-1.f, 90.f, CurrentSpline->Splines[SplineIndex].DOWN);
		return;
	}

	TurnAround();
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
			MoveToNewSpline(1.f, -90.f, CurrentSpline->Splines[SplineIndex].UPWARD);
			return;
		}
		case 1:
		{
			MoveToNewSpline(-1.f, 90.f, CurrentSpline->Splines[SplineIndex].DOWN);
			return;
		}
		case 2:
		{
			MoveToNewSpline(-1.f, 180.f, CurrentSpline->Splines[SplineIndex].LEFT);
			return;
		}
		case 3:
		{
			MoveToNewSpline(1.f, 0.f, CurrentSpline->Splines[SplineIndex].RIGHT);
			return;
		}
		default: return;
	}
}

void APMGhost::ReachingMarkedSpline()
{
	switch (MovementState)
	{
		case EGhostMovementState::ATTACK:
		{
			return;
		}
		case EGhostMovementState::RELEASE:
		{
			MovementState = EGhostMovementState::PASSIVE;
			return;
		}
		case EGhostMovementState::HITTED:
		{
			EndVulnerableState();
			Release();
			return;
		}
		default: return;
	}
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




