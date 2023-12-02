// Fill out your copyright notice in the Description page of Project Settings.
// Made by Dawid Szo³dra

#include "PMPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "PMSpline.h"
#include "Components/SplineComponent.h"

// Sets default values
APMPlayer::APMPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	
	MovingDirection = 1.f;
	PositionOnSpline = 0.f;
	bIsMoving = false;
	SplineIndex = 0;
	TempDirection = NONE;
	DesiredDirection = RIGHT;
	CurrentDirection = RIGHT;
}

// Called when the game starts or when spawned
void APMPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), FName(TEXT("Start")), OutActors);

	for (AActor* item : OutActors)
	{
		CurrentSpline = Cast<APMSpline>(item);
	}

	if (CurrentSpline == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid CurrentSpline"));
		return;
	}

	bIsMoving = true;
}

// Called every frame
void APMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Bool value is: %s"), bIsMoving ? "true" : "false");
	
	if (bIsMoving)
	{
		PositionOnSpline += DeltaTime * MovingDirection * Speed;
	}
	const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
	SetActorLocation(NewLocation);

	if (CheckIfAtPoint(SplineIndex))
	{
		bIsMoving = false;
		ChooseNewSpline();
	}
}

// Called to bind functionality to input
void APMPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Started, this, &APMPlayer::MoveUp);
		EnhancedInputComponent->BindAction(MoveDownAction, ETriggerEvent::Started, this, &APMPlayer::MoveDown);
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &APMPlayer::MoveLeft);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Started, this, &APMPlayer::MoveRight);
	}

}

void APMPlayer::RotatePacman(float Yaw, TEnumAsByte<EDirections> Direction)
{
	CurrentDirection = Direction;
	SetActorRotation(FRotator(0, Yaw, 0));
	MovingDirection = Yaw >= 0 ? 1.f : -1.f;
	bIsMoving = true;
}

void APMPlayer::MoveUp()
{
	DesiredDirection = UPWARD;
	TempDirection = NONE;

	if (CurrentDirection == DOWN)
	{
		RotatePacman(0.f, EDirections::UPWARD);
	}
}

void APMPlayer::MoveDown()
{
	TempDirection = NONE;
	DesiredDirection = DOWN;

	if (CurrentDirection == UPWARD)
	{
		RotatePacman(-180.f, EDirections::DOWN);
	}
}

void APMPlayer::MoveLeft()
{	
	DesiredDirection = LEFT;
	TempDirection = NONE;

	if (CurrentDirection == RIGHT)
	{
		RotatePacman(-90.f, EDirections::LEFT);
	}
}

void APMPlayer::MoveRight()
{
	DesiredDirection = RIGHT;
	TempDirection = NONE;

	if (CurrentDirection == LEFT)
	{
		RotatePacman(90.f, EDirections::RIGHT);
	}
}

void APMPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APMPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

bool APMPlayer::CheckIfAtPoint(int32& SplineIndexOut) const
{
	if (GetActorLocation().Equals(CurrentSpline->SplineComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World), 0.5f))
	{
		SplineIndexOut = 1;
		return true;
	}
	else if (GetActorLocation().Equals(CurrentSpline->SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), 0.5f))
	{
		SplineIndexOut = 0;
		return true;
	}

	return	false;
}

void APMPlayer::ChooseNewSpline()
{
	switch (DesiredDirection)
	{
		case UPWARD:
		{
			if (APMSpline* newSpline = CurrentSpline->Splines[SplineIndex].UPWARD)
			{
				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, 0, 0));
				CurrentSpline = newSpline;
				PositionOnSpline = 1.f;
				CurrentDirection = UPWARD;	
				MovingDirection = 1.f;
				bIsMoving = true;
			}
			else
			{
				TempDirection = DesiredDirection;
				DesiredDirection = CurrentDirection;
			}
			return;
		}
		case DOWN:
		{
			if (APMSpline* newSpline = CurrentSpline->Splines[SplineIndex].DOWN)
			{	
				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, 180, 0));
				CurrentSpline = newSpline;
				PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
				CurrentDirection = DOWN;
				MovingDirection = -1.f;
				bIsMoving = true;
			}
			else
			{
				TempDirection = DesiredDirection;
				DesiredDirection = CurrentDirection;
			}
			return;
		}
		case RIGHT:
		{
			if (APMSpline* newSpline = CurrentSpline->Splines[SplineIndex].RIGHT)
			{
				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, 90, 0));
				CurrentSpline = newSpline;
				PositionOnSpline = 1.f;
				CurrentDirection = RIGHT;
				MovingDirection = 1.f;
				bIsMoving = true;
			}
			else
			{
				TempDirection = DesiredDirection;
				DesiredDirection = CurrentDirection;
			}
			return;
		}
		case LEFT:
		{
			if (APMSpline* newSpline = CurrentSpline->Splines[SplineIndex].LEFT)
			{
				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, -90, 0));
				CurrentSpline = newSpline;
				PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
				CurrentDirection = LEFT;				
				MovingDirection = -1.f;
				bIsMoving = true;
			}
			else
			{
				TempDirection = DesiredDirection;
				DesiredDirection = CurrentDirection;
			}
			return;
		}
		default: return;
	}
}




