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
	CollisionBoxBackward = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Backward"));
	CollisionBoxLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Left"));
	CollisionBoxRight = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Right"));
	CollisionBoxForward->SetupAttachment(Mesh);
	CollisionBoxBackward->SetupAttachment(Mesh);	
	CollisionBoxLeft->SetupAttachment(Mesh);	
	CollisionBoxRight->SetupAttachment(Mesh);	

	bPathAvailableForward = true;
	bPathAvailableBackward = true;
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
	CollisionBoxBackward->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);
	CollisionBoxRight->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);	
	CollisionBoxLeft->OnComponentBeginOverlap.AddDynamic(this, &APMPlayer::OnOverlapBegin);

	CollisionBoxForward->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxBackward->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxRight->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
	CollisionBoxLeft->OnComponentEndOverlap.AddDynamic(this, &APMPlayer::OnOverlapEnd);
}

// Called every frame
void APMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		AddActorLocalOffset(FVector(Speed*DeltaTime,0,0));
		UE_LOG(LogTemp, Warning, TEXT("jest"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("nie ma"));
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
		if ((Rotation == 90 && bPathAvailableLeft) || (Rotation == -90 && bPathAvailableRight) || (Rotation == 180 && bPathAvailableBackward))
		{
			SetActorRotation(FRotator(0, 0, 0));
		}
	}
	else
	{
		if ((Rotation == 90 && bPathAvailableRight) || (Rotation == -90 && bPathAvailableLeft) || (Rotation == 0 && bPathAvailableBackward))
		{
			SetActorRotation(FRotator(0, 180, 0));
		}
	}
}

void APMPlayer::MoveRightLeft(const FInputActionValue& Value)
{	
	const float Rotation = GetActorRotation().Yaw;
	if (Value.Get<float>() == 1.f)
	{
		if ((Rotation == 0 && bPathAvailableRight) || (Rotation == 180 && bPathAvailableLeft) || (Rotation == -90 && bPathAvailableBackward))
		{
			SetActorRotation(FRotator(0, 90, 0));
		}
	}
	else
	{
		if ((Rotation == 0 && bPathAvailableLeft) || (Rotation == 180 && bPathAvailableRight) || (Rotation == 90 && bPathAvailableBackward))
		{
			SetActorRotation(FRotator(0, -90, 0));
		}
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
		else if (OverlappedComp == CollisionBoxBackward)
		{
			bPathAvailableBackward = false;
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
		else if (OverlappedComp == CollisionBoxBackward)
		{
			bPathAvailableBackward = true;
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
