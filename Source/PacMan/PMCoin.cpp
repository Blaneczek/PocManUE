// Fill out your copyright notice in the Description page of Project Settings.


#include "PMCoin.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PMPlayer.h"

// Sets default values
APMCoin::APMCoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);

}

// Called when the game starts or when spawned
void APMCoin::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APMCoin::OnOverlapBegin);
}

// Called every frame
void APMCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APMCoin::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APMPlayer* Player = Cast<APMPlayer>(OtherActor);
	if (Player != nullptr)
	{
		Destroy();
		//TODO: add points
	}
}

