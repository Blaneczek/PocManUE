// Fill out your copyright notice in the Description page of Project Settings.


#include "PMSpline.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
APMSpline::APMSpline()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	SetRootComponent(SceneComponent);
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(SceneComponent);

	Splines.Add(FSplines());
	Splines.Add(FSplines());
}



