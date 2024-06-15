// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <PhysicsEngine/PhysicsThruster.h>
#include <PhysicsEngine/PhysicsThrusterComponent.h>
#include "DroneBase.generated.h"

class USkeletalMeshComponent;
class UStabilizationComponentBase;

USTRUCT(BlueprintType)
struct FActionsUsed
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bVertical = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRotation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bLeftRight = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bFrontBack = false;
};

UCLASS()
class DRONESTABILIZATION_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADroneBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> DroneMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPhysicsThrusterComponent* PhysicsThrusterBR;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPhysicsThrusterComponent* PhysicsThrusterBL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPhysicsThrusterComponent* PhysicsThrusterFL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPhysicsThrusterComponent* PhysicsThrusterFR;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UStabilizationComponentBase> StabilizationComponent;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Engines")
	float FR_TickPower = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Engines")
	float FL_TickPower = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Engines")
	float BR_TickPower = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Engines")
	float BL_TickPower = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Parameters")
	FVector StartLocation;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Stabilization")
	bool bIsStabilizationEnabled = false;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Stabilization")
	bool bIsNeuralStabilization = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Engines")
	bool bIsEnginesActivated = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Engines")
	float ThrustStrengthBase = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Engines")
	float AdditionalRotationPowerFirst = 8.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Engines")
	float AdditionalRotationPowerSecond = 3.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gamepad")
	float DynamicForceFeedbackDivider = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stabilization")
	bool bIsAgent = true;

	UFUNCTION(BlueprintCallable)
	void ActiviteAllEngines(bool bActivate);

	UFUNCTION(BlueprintCallable)
	FVector CalculateCenterOfMass();

	UFUNCTION(BlueprintCallable)
	void ResetDrone();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddRandomImpulse();

	UFUNCTION(BlueprintCallable, Category="Actions")
	void VerticalMovement(float ActionValue, float Magnitude);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RotationMovement(float ActionValue, float Magnitude);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void FrontBackMovement(float ActionValue, float Magnitude, float Limiter);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void LeftRightMovement(float ActionValue, float Magnitude, float Limiter);

	FActionsUsed ActionsUsed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;



	
};
