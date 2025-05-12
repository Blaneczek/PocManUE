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
	TempDirection = EPlayerDirection::NONE;
	DesiredDirection = EPlayerDirection::RIGHT;
	CurrentDirection = EPlayerDirection::RIGHT;
	Tags.Add(FName(TEXT("player")));
}

// Called when the game starts or when spawned
void APMPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnStartGame.AddUObject(this, &APMPlayer::Start);
		GameMode->OnStopGame.AddUObject(this, &APMPlayer::Reset);
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

void APMPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(OpenPauseMenuAction, ETriggerEvent::Started, this, &APMPlayer::OpenPauseMenu);
	}
}

void APMPlayer::Rotate180(EPlayerDirection Direction)
{
	CurrentDirection = Direction;
	MovingDirection *= -1;
	const float NewYaw = GetActorRotation().Yaw + (-180 * MovingDirection);
	SetActorRotation(FRotator(0, NewYaw, 0));
	PositionOnSpline += (MovingDirection * 10); //to be sure that player won't stuck when PositionOnSpline == DistanceAtSplinePoint
	bIsMoving = true;
}

void APMPlayer::MarkSpline()
{
	if (CurrentSpline != nullptr)
	{
		CurrentSpline->Tags.Add(FName(TEXT("player_MarkedSpline")));
	}		
}

void APMPlayer::UnmarkSpline()
{
	if (CurrentSpline != nullptr)
	{
		CurrentSpline->Tags.Remove(FName(TEXT("player_MarkedSpline")));
	}
}

void APMPlayer::Reset()
{
	StopMovement();

	FTimerHandle ResetTimerHandle;
	FTimerDelegate ResetDel;
	ResetDel.BindUObject(this, &APMPlayer::SetActorHiddenInGame, true);
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, ResetDel, 1.f, false);
}

void APMPlayer::Start()
{
	SetActorRotation(FRotator(0, 0, 0));
	MovingDirection = 1.f;
	PositionOnSpline = 1.f;
	SplineIndex = 0;
	TempDirection = EPlayerDirection::NONE;
	DesiredDirection = EPlayerDirection::RIGHT;
	CurrentDirection = EPlayerDirection::RIGHT;

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
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		IPMInteractionInterface* InteractionInterface = Cast<IPMInteractionInterface>(OtherActor);
		if (InteractionInterface)
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
		default: return;
	}

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




