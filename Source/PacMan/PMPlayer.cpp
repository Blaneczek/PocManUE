// Fill out your copyright notice in the Description page of Project Settings.


#include "PMPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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

	if (CurrentSpline == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid CurrentSpline in Tick"));
		return;		
	}

	if (bIsMoving)
	{
		PositionOnSpline += DeltaTime * MovingDirection * Speed;
		const FVector NewLocation = CurrentSpline->SplineComponent->GetLocationAtDistanceAlongSpline(PositionOnSpline, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);

		if (CheckIfAtPoint())
		{
			bIsMoving = false;
		}
	}
	else
	{
		ChooseNewSpline();
	}

}

// Called to bind functionality to input
void APMPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveUpDownAction, ETriggerEvent::Started, this, &APMPlayer::MoveUpDown);
		EnhancedInputComponent->BindAction(MoveRightLeftAction, ETriggerEvent::Started, this, &APMPlayer::MoveRightLeft);
	}

}

void APMPlayer::MoveUpDown(const FInputActionValue& Value)
{
	if (Value.Get<float>() == 1.f)
	{
		switch (CurrentDirection)
		{
			case EDirections::DOWN:
			{
				CurrentDirection = EDirections::UPWARD;
				SetActorRotation(FRotator(0, 0, 0));
				MovingDirection = 1.f;
				bIsMoving = true;
				break;
			}
			case EDirections::LEFT:
			{
				DesiredDirection = EDirections::UPWARD;
				break;
			}
			case EDirections::RIGHT:
			{
				DesiredDirection = EDirections::UPWARD;
				break;
			}
			default: break;
		}
	}
	else
	{
		switch (CurrentDirection)
		{
			case EDirections::UPWARD:
			{
				CurrentDirection = EDirections::DOWN;
				SetActorRotation(FRotator(0, 180, 0));
				MovingDirection = -1.f;
				bIsMoving = true;
				break;
			}
			case EDirections::LEFT:
			{
				DesiredDirection = EDirections::DOWN;
				break;
			}
			case EDirections::RIGHT:
			{
				DesiredDirection = EDirections::DOWN;
				break;
			}
			default: break;
		}
	}
}

void APMPlayer::MoveRightLeft(const FInputActionValue& Value)
{	
	if (Value.Get<float>() == 1.f)
	{
		switch (CurrentDirection)
		{
			case LEFT:
			{
				CurrentDirection = EDirections::RIGHT;
				SetActorRotation(FRotator(0, 90, 0));
				MovingDirection = 1.f;
				bIsMoving = true;
				break;
			}
			case UPWARD:
			{
				DesiredDirection = RIGHT;
				break;
			}
			case DOWN:
			{
				DesiredDirection = RIGHT;
				break;
			}
			default: break;
		}
	}
	else
	{
		switch (CurrentDirection)
		{
		case RIGHT:
		{
			CurrentDirection = LEFT;
			SetActorRotation(FRotator(0, -90, 0));
			MovingDirection = -1.f;
			bIsMoving = true;
			break;
		}
		case UPWARD:
		{
			DesiredDirection = LEFT;
			break;
		}
		case DOWN:
		{
			DesiredDirection = LEFT;
			break;
		}
		default: break;
		}
	}
}

void APMPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APMPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

bool APMPlayer::CheckIfAtPoint() const
{
	return	GetActorLocation().Equals(CurrentSpline->SplineComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World), 0) 
			|| GetActorLocation().Equals(CurrentSpline->SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World), 0);
}

void APMPlayer::ChooseNewSpline()
{
	switch (CurrentDirection)
	{
		case UPWARD:
		{
			switch (DesiredDirection)
			{
				case NONE:
				{
					APMSpline* newSpline = CurrentSpline->Splines[1].UPWARD;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = 1.f;
						bIsMoving = true;
					}
					break;
				}
				case RIGHT:
				{
					APMSpline* newSpline = CurrentSpline->Splines[1].RIGHT;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = 1.f;
						DesiredDirection = NONE;
						CurrentDirection = RIGHT;
						SetActorRotation(FRotator(0, 90, 0));
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[1].UPWARD;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = 1.f;
							bIsMoving = true;
						}
					}
					break;
				}
				case LEFT:
				{
					APMSpline* newSpline = CurrentSpline->Splines[1].LEFT;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
						DesiredDirection = NONE;
						CurrentDirection = LEFT;
						SetActorRotation(FRotator(0, -90, 0));
						MovingDirection = -1.f;
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[1].UPWARD;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = 1.f;
							bIsMoving = true;
						}
					}
					break;
				}
				default: break;
			}
			break;
		}
		case DOWN:
		{
			switch (DesiredDirection)
			{
				case NONE:
				{
					APMSpline* newSpline = CurrentSpline->Splines[0].DOWN;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
						bIsMoving = true;
					}
					break;
				}
				case RIGHT:
				{
					APMSpline* newSpline = CurrentSpline->Splines[0].RIGHT;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = 1.f;
						DesiredDirection = NONE;
						CurrentDirection = RIGHT;
						SetActorRotation(FRotator(0, 90, 0));
						MovingDirection = 1.f;
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[0].DOWN;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
							bIsMoving = true;
						}
					}
					break;
				}
				case LEFT:
				{
					APMSpline* newSpline = CurrentSpline->Splines[0].LEFT;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
						DesiredDirection = NONE;
						CurrentDirection = LEFT;
						SetActorRotation(FRotator(0, -90, 0));
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[0].DOWN;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
							bIsMoving = true;
						}
					}
					break;
				}
				default: break;
			}
			break;
		}
		case LEFT:
		{
			switch (DesiredDirection)
			{
				case NONE:
				{
					APMSpline* newSpline = CurrentSpline->Splines[0].LEFT;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
						bIsMoving = true;
					}
					break;
				}
				case UPWARD:
				{
					APMSpline* newSpline = CurrentSpline->Splines[0].UPWARD;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = 1.f;
						DesiredDirection = NONE;
						CurrentDirection = UPWARD;
						SetActorRotation(FRotator(0, 0, 0));
						MovingDirection = 1.f;
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[0].LEFT;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
							bIsMoving = true;
						}
					}
					break;
				}
				case DOWN:
				{
					APMSpline* newSpline = CurrentSpline->Splines[0].DOWN;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
						DesiredDirection = NONE;
						CurrentDirection = DOWN;
						SetActorRotation(FRotator(0, 180, 0));
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[0].LEFT;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
							bIsMoving = true;
						}
					}
					break;
				}
				default: break;
			}
			break;
		}
		case RIGHT:
		{
			switch (DesiredDirection)
			{
				case NONE:
				{
					APMSpline* newSpline = CurrentSpline->Splines[1].RIGHT;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = 1.f;
						bIsMoving = true;
					}
					break;
				}
				case UPWARD:
				{
					APMSpline* newSpline = CurrentSpline->Splines[1].UPWARD;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = 1.f;
						DesiredDirection = NONE;
						CurrentDirection = UPWARD;
						SetActorRotation(FRotator(0, 0, 0));
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[1].RIGHT;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = 1.f;
							bIsMoving = true;
						}
					}
					break;
				}
				case DOWN:
				{
					APMSpline* newSpline = CurrentSpline->Splines[1].DOWN;
					if (newSpline != nullptr)
					{
						CurrentSpline = newSpline;
						PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1;
						DesiredDirection = NONE;
						CurrentDirection = DOWN;
						SetActorRotation(FRotator(0, 180, 0));
						MovingDirection = -1.f;
						bIsMoving = true;
					}
					else
					{
						newSpline = CurrentSpline->Splines[1].RIGHT;
						if (newSpline != nullptr)
						{
							CurrentSpline = newSpline;
							PositionOnSpline = 1.f;
							bIsMoving = true;
						}
					}
					break;
				}
				default: break;
			}
			break;
		}
		default: break;
	}
}


