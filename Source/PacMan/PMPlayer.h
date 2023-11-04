// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PMPlayer.generated.h"

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
	class UBoxComponent* CollisionBoxForward;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxBackward;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxLeft;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBoxRight;

	UPROPERTY(EditAnywhere)
	float Speed = 50.f;

	bool bPathAvailableForward;
	bool bPathAvailableBackward;
	bool bPathAvailableLeft;
	bool bPathAvailableRight;
	bool bIsMoving;

};
