// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DroneBase.h"
#include "StabilizationComponentBase.generated.h"

UENUM(BlueprintType)
enum class EActions : uint8
{
	Vertical,
	Rotation,
	LeftRight,
	FrontBack
};

USTRUCT(BlueprintType)
struct FStabilizationParametersPID
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float P = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float I = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float D = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Bias = 0.0f;
};

USTRUCT(BlueprintType)
struct FAxisDeactivated
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
struct FPIDVariables
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VerticalIntegral = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VerticalErrorPrior = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationIntegral = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationErrorPrior = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FrontBackIntegral = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FrontBackErrorPrior = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LeftRightIntegral = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LeftRightErrorPrior = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaFL = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaFR = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaBL = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaBR = 0.0f;
};

USTRUCT(BlueprintType)
struct FMLVariables
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaFL = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaFR = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaBL = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DeltaBR = 0.0f;
};

USTRUCT(BlueprintType)
struct FSmoothParametersPID
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float VelocityDivider = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Interval1 = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Interval2 = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SubtrahendDetlaChanger = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DividerDetlaChanger = 0.0f;

};

USTRUCT(BlueprintType)
struct FSmoothParametersML
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Divider = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Min = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Max = 50.0f;

};



UCLASS(Blueprintable)
class DRONESTABILIZATION_API UStabilizationComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStabilizationComponentBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Stabilization")
	FAxisDeactivated AxisDeactivated;

	UFUNCTION(BlueprintCallable)
	void SetAxisDeactivated(bool bIsDeactivated, EActions Axis);

	UFUNCTION(BlueprintCallable)
	void ResetAxisDeactivated();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Stabilization")
	FPIDVariables PIDVariables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Stabilization")
	FMLVariables MLVariables;

	UFUNCTION(BlueprintCallable)
	float PIDFuntion(float CurrentValue, float DesiredValue, FStabilizationParametersPID Parameters, float DeltaTime, float& Integral, float& ErrorPrior);

	UFUNCTION(BlueprintCallable)
	void VerticalPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max);

	UFUNCTION(BlueprintCallable)
	void RotationPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max);

	UFUNCTION(BlueprintCallable)
	void FrontBacklPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max, FSmoothParametersPID SmoothParameters);

	UFUNCTION(BlueprintCallable)
	void LeftRightPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max, FSmoothParametersPID SmoothParameters);

	UFUNCTION(BlueprintCallable)
	void ApplyPID(ADroneBase* Drone, float Scale, float Min, float Max);

	UFUNCTION(BlueprintCallable)
	void AllAxisML(ADroneBase* Drone, EActions Axis, float FL, float FR, float BL, float BR, float Multiplier, float Min, float Max, FSmoothParametersML SmoothParameters);

	UFUNCTION(BlueprintCallable)
	float CheckRotationML(ADroneBase* Drone, EActions Axis, float Divider, float Min, float Max);

	UFUNCTION(BlueprintCallable)
	void ApplyML(ADroneBase* Drone, float Scale, float Delta, float Min, float Max);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

		
	
};
