// Copyright (c) 2024 Dawid Szoldra. All rights reserved.

#include "PMPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Splines/PMSpline.h"
#include "Components/SplineComponent.h"
#include "GameModes/Gameplay/PMGameModeBase.h"
#include "Interfaces/PMInteractionInterface.h"

// Sets default values
APMPlayer::APMPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	
	Speed = 500.f;
	MovingDirection = 1.f;
	PositionOnSpline = 1.f;
	bIsMoving = false;
	SplineIndex = 0;
	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::RIGHT;
	CurrentDirection = EDirection::RIGHT;
	Tags.Add(TEXT("player"));
}

// Called when the game starts or when spawned
void APMPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnStartGame.AddUObject(this, &APMPlayer::StartPlayer);
		GameMode->OnStopGame.AddUObject(this, &APMPlayer::ResetPlayer);
		GameMode->OnStartMovement.AddUObject(this, &APMPlayer::StartMovement);
		GameMode->OnStopMovement.AddUObject(this, &APMPlayer::StopMovement);		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::BeginPlay | GameMode is nullptr"));
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{	
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
		DisableInput(PC);
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
}

// Called every frame
void APMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSpline == nullptr)
	{
		return;
	}

	if (bIsMoving)
	{
		PositionOnSpline += DeltaTime * MovingDirection * Speed;
	}

	const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
	SetActorLocation(NewLocation);

	if (CheckIfAtSplinePoint())
	{
		UnmarkSpline();
		bIsMoving = false;
		ChooseNewSpline();
		MarkSpline();
	}
}

void APMPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(OpenPauseMenuAction, ETriggerEvent::Started, this, &APMPlayer::OpenPauseMenu);
	}
}

void APMPlayer::RotatePlayer(float Yaw, EDirection Direction)
{
	CurrentDirection = Direction;
	SetActorRotation(FRotator(0, Yaw, 0));
	MovingDirection = Yaw <= 0 ? 1.f : -1.f;
	PositionOnSpline += (MovingDirection * 5); //to be sure that player won't stuck when PositionOnSpline == DistanceAtSplinePoint
	bIsMoving = true;
}

void APMPlayer::MarkSpline()
{
	if (CurrentSpline != nullptr)
	{
		CurrentSpline->Tags.Add(FName("player_MarkedSpline"));
	}		
}

void APMPlayer::UnmarkSpline()
{
	if (CurrentSpline != nullptr)
	{
		CurrentSpline->Tags.Remove(FName("player_MarkedSpline"));
	}
}

void APMPlayer::ResetPlayer()
{
	StopMovement();

	FTimerHandle ResetTimerHandle;
	FTimerDelegate ResetDel;
	ResetDel.BindUObject(this, &APMPlayer::SetActorHiddenInGame, true);
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, ResetDel, 1.f, false);
}

void APMPlayer::StartPlayer()
{
	SetActorRotation(FRotator(0, 0, 0));
	MovingDirection = 1.f;
	PositionOnSpline = 1.f;
	SplineIndex = 0;
	TempDirection = EDirection::NONE;
	DesiredDirection = EDirection::RIGHT;
	CurrentDirection = EDirection::RIGHT;

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), FName(TEXT("start")), OutActors);
	for (AActor* item : OutActors)
	{
		CurrentSpline = Cast<APMSpline>(item);
	}

	if (CurrentSpline == nullptr)
	{		
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::BeginPlay | CurrentSpline is nullptr"));
		return;
	}

	MarkSpline();
	const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
	SetActorLocation(NewLocation);
	
	SetActorHiddenInGame(false);
}

void APMPlayer::StartMovement()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);		
		bIsMoving = true;
	}
}

void APMPlayer::StopMovement()
{
	bIsMoving = false;	

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		this->DisableInput(PC);	
	}
}

void APMPlayer::OpenPauseMenu()
{
	if (GameMode != nullptr)
	{
		GameMode->OpenPauseMenu();
	}	
}

void APMPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( (OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		IPMInteractionInterface* InteractionInterface = Cast<IPMInteractionInterface>(OtherActor);
		if (InteractionInterface != nullptr)
		{
			GameMode->AddPoints(InteractionInterface->Interaction());
		}
	}
}

bool APMPlayer::CheckIfAtSplinePoint()
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

void APMPlayer::ChooseNewSpline()
{
	APMSpline* NewSpline = nullptr;

	switch (DesiredDirection)
	{
		case EDirection::UPWARD:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
			break;
		}		
		case EDirection::DOWN:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].DOWN;
			break;
		}			
		case EDirection::RIGHT:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
			break;
		}			
		case EDirection::LEFT:
		{
			NewSpline = CurrentSpline->Splines[SplineIndex].LEFT;
			break;
		}		
		default: return;
	}

	if (!NewSpline || NewSpline->ActorHasTag(FName("releaseGhost")))
	{
		TempDirection = DesiredDirection;
		DesiredDirection = CurrentDirection;
		return;
	}

	float NewYaw = 0.f;
	if (DesiredDirection == EDirection::UPWARD)
	{
		NewYaw = -90.f;
	}
	else if (DesiredDirection == EDirection::DOWN)
	{
		NewYaw = 90.f;
	}	
	else if (DesiredDirection == EDirection::LEFT)
	{
		NewYaw = 180.f;
	}
	
	SetActorRotation(FRotator(0, NewYaw, 0));
	CurrentSpline = NewSpline;
	PositionOnSpline = (DesiredDirection == EDirection::UPWARD || DesiredDirection == EDirection::RIGHT) ? 1.f :
						CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
	CurrentDirection = DesiredDirection;
	MovingDirection = (DesiredDirection == EDirection::UPWARD || DesiredDirection == EDirection::RIGHT) ? 1.f : -1.f;
	bIsMoving = true;

	if (TempDirection != EDirection::NONE)
	{
		DesiredDirection = TempDirection;
	}
}




