// Fill out your copyright notice in the Description page of Project Settings.


#include "PMPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
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
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	SceneComponent->SetupAttachment(Mesh);
	SceneComponent->SetUsingAbsoluteRotation(true);
	CollisionBoxFront = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Front"));
	CollisionBoxFront->SetupAttachment(Mesh);

	CollisionBoxTop = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Top"));
	CollisionBoxLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Left"));
	CollisionBoxRight = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Right"));
	CollisionBoxBottom = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Bottom"));
	CollisionBoxTop->SetupAttachment(SceneComponent);
	CollisionBoxLeft->SetupAttachment(SceneComponent);
	CollisionBoxRight->SetupAttachment(SceneComponent);
	CollisionBoxBottom->SetupAttachment(SceneComponent);

	bPathAvailableTop = true;
	bPathAvailableRight = true;
	bPathAvailableLeft = true;
	bPathAvailableBottom = true;

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

	CollisionBoxFront->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
	CollisionBoxTop->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
	CollisionBoxRight->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);	
	CollisionBoxLeft->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
	CollisionBoxBottom->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);

	CollisionBoxFront->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxTop->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxRight->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxLeft->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxBottom->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
}

// Called every frame
void APMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	switch (InputDirection)
	{
		case EInputDirections::TOP:
			if (bPathAvailableTop)
			{
				SetActorRotation(FRotator(0, 0, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("UPWARD"));
			}
			/*if ((Rotation == 90 && bPathAvailableLeft) || (Rotation == -90 && bPathAvailableRight) || (Rotation == 180))
			{
				SetActorRotation(FRotator(0, 0, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("UPWARD"));
			}*/
			break;

		case EInputDirections::BOTTOM:
			if (bPathAvailableBottom)
			{
				SetActorRotation(FRotator(0, 180, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("BOTTOM"));
			}
			/*if ((Rotation == 90 && bPathAvailableRight) || (Rotation == -90 && bPathAvailableLeft) || (Rotation == 0))
			{
				SetActorRotation(FRotator(0, 180, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("DOWN"));
			}*/
			break;

		case EInputDirections::RIGHT:
			if (bPathAvailableRight)
			{
				SetActorRotation(FRotator(0, 90, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
			}
			/*if ((Rotation == 0 && bPathAvailableRight) || (Rotation == 180 && bPathAvailableLeft) || (Rotation == -90))
			{
				SetActorRotation(FRotator(0, 90, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
			}*/
			break;

		case EInputDirections::LEFT:
			if (bPathAvailableLeft)
			{
				SetActorRotation(FRotator(0, -90, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("LEFT"));
			}
			/*if ((Rotation == 0 && bPathAvailableLeft) || (Rotation == 180 && bPathAvailableRight) || (Rotation == 90))
			{
				SetActorRotation(FRotator(0, -90, 0));
				InputDirection = EInputDirections::NONE;
				UE_LOG(LogTemp, Warning, TEXT("LEFT"));
			}*/
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
		InputDirection = EInputDirections::TOP;
	}
	else
	{
		InputDirection = EInputDirections::BOTTOM;
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
	if (OtherActor != nullptr && OtherActor != this && OverlappedComp != nullptr)
	{
		if (OverlappedComp == CollisionBoxFront)
		{
			bIsMoving = false;
		}
		else if (OverlappedComp == CollisionBoxTop)
		{
			bPathAvailableTop = false;
		}
		else if (OverlappedComp == CollisionBoxRight)
		{
			bPathAvailableRight = false;
		}
		else if (OverlappedComp == CollisionBoxLeft)
		{
			bPathAvailableLeft = false;
		}
		else if (OverlappedComp == CollisionBoxBottom)
		{
			bPathAvailableBottom = false;
		}
	}
}

void APMPlayer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OverlappedComp != nullptr)
	{
		if (OverlappedComp == CollisionBoxFront)
		{
			bIsMoving = true;
		}
		else if (OverlappedComp == CollisionBoxTop)
		{
			bPathAvailableTop = true;
		}
		else if (OverlappedComp == CollisionBoxRight)
		{
			bPathAvailableRight = true;
		}
		else if (OverlappedComp == CollisionBoxLeft)
		{
			bPathAvailableLeft = true;
		}
		else if (OverlappedComp == CollisionBoxBottom)
		{
			bPathAvailableBottom = true;
		}
	}
}
