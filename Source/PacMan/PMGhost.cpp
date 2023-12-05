// Fill out your copyright notice in the Description page of Project Settings.


#include "PMGhost.h"
#include "Kismet/GameplayStatics.h"
#include "PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APMGhost::APMGhost()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	MovingDirection = 1.f;
	PositionOnSpline = 0.f;
	SplineIndex = 0;
}

// Called when the game starts or when spawned
void APMGhost::BeginPlay()
{
	Super::BeginPlay();
	
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

	int32 randomIndex = FMath::RandRange(0, counter - 1);

	if (counter == 0)
	{
		MovingDirection *= -1.f;
		return;
	}


	switch (validSplines[randomIndex])
	{
		case 0:
		{
			CurrentSpline = CurrentSpline->Splines[SplineIndex].UPWARD;
			PositionOnSpline = 1.f;
			MovingDirection = 1.f;
			return;
		}
		case 1:
		{
			CurrentSpline = CurrentSpline->Splines[SplineIndex].DOWN;
			PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
			MovingDirection = -1.f;
			return;
		}
		case 2:
		{
			CurrentSpline = CurrentSpline->Splines[SplineIndex].LEFT;
			PositionOnSpline = CurrentSpline->SplineComponent->GetDistanceAlongSplineAtSplinePoint(1) - 1.f;
			MovingDirection = -1.f;
			return;
		}
		case 3:
		{
			CurrentSpline = CurrentSpline->Splines[SplineIndex].RIGHT;
			PositionOnSpline = 1.f;
			MovingDirection = 1.f;
			return;
		}
		default: return;
	}
}


