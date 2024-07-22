// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <PhysicsEngine/PhysicsThruster.h>
#include <PhysicsEngine/PhysicsThrusterComponent.h>
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemReplicationProxyInterface.h"
#include "DroneDataTypes.h"
#include "DroneBase.generated.h"

class USkeletalMeshComponent;
class UStabilizationComponentBase;
class UAbilitySystemComponentBase;
class UAttributeSetBase;
class UGameplayAbility;
class UGameplayEffect;


UENUM(BlueprintType)
enum class EActions : uint8
{
	Vertical,
	Rotation,
	LeftRight,
	FrontBack
};

USTRUCT(BlueprintType)
struct FActionsUsed
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bVertical = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRotation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bLeftRight = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bFrontBack = false;
};

USTRUCT(BlueprintType)
struct FDesiredMovementValues
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Vertical = 0.0f;

	// Yaw (Z rotation)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Rotation = 0.0f;

	// Pitch (Y rotation)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FrontBack = 0.0f;

	// Roll (X rotation)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LeftRight = 0.0f;
};

UCLASS()
class DRONESTABILIZATION_API ADroneBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADroneBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	FDroneData GetDroneData() const;

	UFUNCTION(BlueprintCallable)
	void SetDroneData(const FDroneData& InDroneData);

	UFUNCTION(BlueprintCallable)
	void ActiviteAllEngines(bool bActivate);

	UFUNCTION(BlueprintCallable)
	FVector CalculateCenterOfMass();

	UFUNCTION(BlueprintCallable)
	void ResetDrone();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddRandomImpulse();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void VerticalMovement(float ActionValue, float Magnitude, float Limiter);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RotationMovement(float ActionValue, float Magnitude, float Limiter);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void FrontBackMovement(float ActionValue, float Magnitude, float Limiter);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void LeftRightMovement(float ActionValue, float Magnitude, float Limiter);

	UFUNCTION(BlueprintCallable)
	void SetDesiredValues(EActions Axis, float Value);



protected:

	virtual void BeginPlay() override;

	void GiveAbilities();

	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION()
	void OnRep_DroneData();

	virtual void InitFromDroneData(const FDroneData InDroneData, bool bFromReplication = false);

public:
	FActionsUsed ActionsUsed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,Replicated)
	FDesiredMovementValues DesiredValues;

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

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Engines")
	float VerticalAdditionalPower = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Engines")
	float RotationAdditionalPower = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Parameters")
	FVector StartLocation;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Stabilization")
	bool bIsStabilizationEnabled = false;

	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Stabilization")
	bool bIsNeuralStabilization = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Engines")
	bool bIsEnginesActivated = false;

protected:
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

	UPROPERTY(EditDefaultsOnly)
	UAbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UAttributeSetBase* AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_DroneData)
	FDroneData DroneData;

	UPROPERTY(EditDefaultsOnly)
	class UDroneDataAsset* DroneDataAsset;
};
