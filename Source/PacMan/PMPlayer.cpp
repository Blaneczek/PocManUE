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
#include "PMGameModeBase.h"
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
	
	MovingDirection = 1.f;
	PositionOnSpline = 1.f;
	bIsMoving = false;
	SplineIndex = 0;
	TempDirection = NONE;
	DesiredDirection = RIGHT;
	CurrentDirection = RIGHT;
	bChased = false;
}

// Called when the game starts or when spawned
void APMPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<APMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::BeginPlay | GameMode is nullptr"));
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{	
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
		DisableInput(PC);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::BeginPlay | PlayerController is nullptr"));
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
}

// Called every frame
void APMPlayer::Tick(float DeltaTime)
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
		UnmarkSpline();
		bIsMoving = false;
		ChooseNewSpline();
		MarkSpline();
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

void APMPlayer::RotatePlayer(float Yaw, TEnumAsByte<EDirections> Direction)
{
	CurrentDirection = Direction;
	SetActorRotation(FRotator(0, Yaw, 0));
	MovingDirection = Yaw <= 0 ? 1.f : -1.f;
	bIsMoving = true;
}

void APMPlayer::MarkSpline()
{
	CurrentSpline->Tags.Add(FName("markedSpline"));
	bChased = true;
}

void APMPlayer::UnmarkSpline()
{
	CurrentSpline->Tags.Remove(FName("markedSpline"));
}

void APMPlayer::ResetPlayer()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		DisableInput(PC);
		UE_LOG(LogTemp, Warning, TEXT("jest controller"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("nie ma controllera"));
	}

	bIsMoving = false;
	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &APMPlayer::HidePlayer, 1.f, false);
}

void APMPlayer::HidePlayer()
{
	SetActorHiddenInGame(true);
}

void APMPlayer::StartPlayer()
{
	SetActorRotation(FRotator(0, 0, 0));
	MovingDirection = 1.f;
	PositionOnSpline = 1.f;
	SplineIndex = 0;
	TempDirection = EDirections::NONE;
	DesiredDirection = EDirections::RIGHT;
	CurrentDirection = EDirections::RIGHT;
	bChased = false;
	this->SetActorHiddenInGame(false);

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APMSpline::StaticClass(), FName(TEXT("Start")), OutActors);
	for (AActor* item : OutActors)
	{
		CurrentSpline = Cast<APMSpline>(item);
	}

	if (CurrentSpline == nullptr)
	{		
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::BeginPlay | CurrentSpline is nullptr"));
		return;
	}

	const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
	SetActorLocation(NewLocation);
}

void APMPlayer::StartMovement()
{
	bIsMoving = true;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		this->EnableInput(PC);		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::StartMovement | PlayerController is nullptr"));
	}
	
}

void APMPlayer::StopMovement()
{
	bIsMoving = false;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		this->DisableInput(PC);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APMPlayer::StopMovement | PlayerController is nullptr"));
	}
}

void APMPlayer::MoveUp()
{
	if (CurrentDirection == UPWARD) return;

	TempDirection = NONE;
	DesiredDirection = UPWARD;
	
	if (CurrentDirection == DOWN)
	{
		RotatePlayer(-90.f, EDirections::UPWARD);
	}
}

void APMPlayer::MoveDown()
{
	if (CurrentDirection == DOWN) return;

	TempDirection = NONE;
	DesiredDirection = DOWN;

	if (CurrentDirection == UPWARD)
	{
		RotatePlayer(90.f, EDirections::DOWN);
	}
}

void APMPlayer::MoveLeft()
{	
	if (CurrentDirection == LEFT) return;

	TempDirection = NONE;
	DesiredDirection = LEFT;
	
	if (CurrentDirection == RIGHT)
	{
		RotatePlayer(180.f, EDirections::LEFT);
	}
}

void APMPlayer::MoveRight()
{
	if (CurrentDirection == RIGHT) return;

	TempDirection = NONE;
	DesiredDirection = RIGHT;
	
	if (CurrentDirection == LEFT)
	{
		RotatePlayer(0.f, EDirections::RIGHT);
	}
}

void APMPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	IPMInteractionInterface* InteractionInterface = Cast<IPMInteractionInterface>(OtherActor);
	if (InteractionInterface != nullptr)
	{
		GameMode->AddPoints(InteractionInterface->Interaction());
	}
}

bool APMPlayer::CheckIfAtPoint()
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
	switch (DesiredDirection)
	{
		case UPWARD:
		{
			if (APMSpline* newSpline = CurrentSpline->Splines[SplineIndex].UPWARD)
			{
				if (CurrentSpline->Splines[SplineIndex].UPWARD->ActorHasTag(FName("releaseGhost")))
				{
					TempDirection = DesiredDirection;
					DesiredDirection = CurrentDirection;
					return;
				}

				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, -90, 0));
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
				if (CurrentSpline->Splines[SplineIndex].DOWN->ActorHasTag(FName("releaseGhost")))
				{
					TempDirection = DesiredDirection;
					DesiredDirection = CurrentDirection;
					return;
				}

				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, 90, 0));
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
				if (CurrentSpline->Splines[SplineIndex].RIGHT->ActorHasTag(FName("releaseGhost")))
				{
					TempDirection = DesiredDirection;
					DesiredDirection = CurrentDirection;
					return;
				}

				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, 0, 0));
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
				if (CurrentSpline->Splines[SplineIndex].LEFT->ActorHasTag(FName("releaseGhost")))
				{
					TempDirection = DesiredDirection;
					DesiredDirection = CurrentDirection;
					return;
				}

				if (TempDirection != NONE)
				{
					DesiredDirection = TempDirection;
				}
				SetActorRotation(FRotator(0, 180, 0));
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




