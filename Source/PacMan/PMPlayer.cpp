// Fill out your copyright notice in the Description page of Project Settings.


#include "PMPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
APMPlayer::APMPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	RootComponent = CollisionSphere;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	CollisionBoxForward = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Forward"));
	CollisionBoxLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Left"));
	CollisionBoxRight = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Right"));
	CollisionBoxForward->SetupAttachment(Mesh);	
	CollisionBoxLeft->SetupAttachment(Mesh);	
	CollisionBoxRight->SetupAttachment(Mesh);	

	bPathAvailableForward = true;
	bPathAvailableRight = true;
	bPathAvailableLeft = true;

	bIsMoving = true;

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

	CollisionBoxForward->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
	CollisionBoxRight->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);	
	CollisionBoxLeft->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);

	CollisionBoxForward->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxRight->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxLeft->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
}

// Called every frame
void APMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Rotation = GetActorRotation().Yaw;
	switch (InputDirection)
	{
		case EInputDirections::UPWARD:
			if ((Rotation == 90 && bPathAvailableLeft) || (Rotation == -90 && bPathAvailableRight) || (Rotation == 180))
			{
				SetActorRotation(FRotator(0, 0, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("UPWARD"));
			}
			break;

		case EInputDirections::DOWN:
			if ((Rotation == 90 && bPathAvailableRight) || (Rotation == -90 && bPathAvailableLeft) || (Rotation == 0))
			{
				SetActorRotation(FRotator(0, 180, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("DOWN"));
			}
			break;

		case EInputDirections::RIGHT:
			if ((Rotation == 0 && bPathAvailableRight) || (Rotation == 180 && bPathAvailableLeft) || (Rotation == -90))
			{
				SetActorRotation(FRotator(0, 90, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
			}
			break;

		case EInputDirections::LEFT:
			if ((Rotation == 0 && bPathAvailableLeft) || (Rotation == 180 && bPathAvailableRight) || (Rotation == 90))
			{
				SetActorRotation(FRotator(0, -90, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("LEFT"));
			}
			break;

		default: break;

	}

	if (bIsMoving)
	{
		AddActorLocalOffset(FVector(Speed*DeltaTime, 0 , 0));
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
	const float Rotation = GetActorRotation().Yaw;
	if (Value.Get<float>() == 1.f)
	{
		InputDirection = EInputDirections::UPWARD;
	}
	else
	{
		InputDirection = EInputDirections::DOWN;
	}
}

void APMPlayer::MoveRightLeft(const FInputActionValue& Value)
{	
	const float Rotation = GetActorRotation().Yaw;
	if (Value.Get<float>() == 1.f)
	{
		InputDirection = EInputDirections::RIGHT;
	}
	else
	{
		InputDirection = EInputDirections::LEFT;
	}
}

void APMPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OverlappedComp == CollisionBoxForward)
		{
			bIsMoving = false;
		}
		else if (OverlappedComp == CollisionBoxRight)
		{
			bPathAvailableRight = false;
		}
		else if (OverlappedComp == CollisionBoxLeft)
		{
			bPathAvailableLeft = false;
		}
	}
}

void APMPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OverlappedComp == CollisionBoxForward)
		{
			bIsMoving = true;
		}
		else if (OverlappedComp == CollisionBoxRight)
		{
			bPathAvailableRight = true;
		}
		else if (OverlappedComp == CollisionBoxLeft)
		{
			bPathAvailableLeft = true;
		}
	}
}
