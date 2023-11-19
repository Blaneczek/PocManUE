// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMPlayer.generated.h"


UENUM()
enum EInputDirections
{
	NONE	UMETA(DisplayName = "None"),
	TOP		UMETA(DisplayName = "Top"),
	BOTTOM	UMETA(DisplayName = "Bottom"),
	LEFT	UMETA(DisplayName = "Left"),
	RIGHT	UMETA(DisplayName = "Right")
};

UCLASS()
class PACMAN_API APMPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APMPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//MappingContext
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* MoveUpDownAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputAction* MoveRightLeftAction;

	UFUNCTION()
	void MoveUpDown(const FInputActionValue& Value);
	UFUNCTION()
	void MoveRightLeft(const FInputActionValue& Value);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxTop;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxBottom;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxLeft;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxRight;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxFront;

	UPROPERTY(EditAnywhere)
	float Speed = 50.f;

	bool bPathAvailableTop;
	bool bPathAvailableBottom;
	bool bPathAvailableLeft;
	bool bPathAvailableRight;
	bool bIsMoving;


	EInputDirections InputDirection = EInputDirections::NONE;
};
