// Fill out your copyright notice in the Description page of Project Settings.


#include "StabilizationComponentBase.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UStabilizationComponentBase::UStabilizationComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UStabilizationComponentBase::SetAxisDeactivated(bool bIsDeactivated, EActions Axis)
{
	switch (Axis)
	{
	case EActions::Vertical:
		AxisDeactivated.bVertical = bIsDeactivated;
		break;
	case EActions::Rotation:
		AxisDeactivated.bRotation = bIsDeactivated;
		break;
	case EActions::LeftRight:
		AxisDeactivated.bLeftRight = bIsDeactivated;
		break;
	case EActions::FrontBack:
		AxisDeactivated.bFrontBack = bIsDeactivated;
		break;
	}
}

// Called when the game starts
void UStabilizationComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

float UStabilizationComponentBase::PIDFuntion(float CurrentValue, FStabilizationParametersPID Parameters, float DeltaTime, float& Integral, float& ErrorPrior)
{
	
	//Original variant from tutorial
	/*
	float PErrorPrior = ErrorPrior;
	float PIntegral = Integral;
	float PError = CurrentValue - Parameters.DesiredValue;
	PIntegral += CurrentValue * DeltaTime;
	float PDerivative = (PError - PErrorPrior) / DeltaTime;
	Integral = PIntegral;
	ErrorPrior = PError;
	return -(Parameters.P * PError + Parameters.I * PIntegral + Parameters.D * PDerivative + Parameters.Bias);*/

	const float PError = CurrentValue - Parameters.DesiredValue;
	Integral += PError * DeltaTime;
	const float PDerivative = (PError - ErrorPrior) / DeltaTime;
	ErrorPrior = PError;

	return -(Parameters.P * PError + Parameters.I * Integral + Parameters.D * PDerivative + Parameters.Bias);
}

void UStabilizationComponentBase::VerticalPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max)
{
	if (!AxisDeactivated.bVertical)
	{		
		float StabilizationDelta = FMath::Clamp((PIDFuntion(Drone->GetVelocity().Z, Parameters, DeltaTime, PIDVariables.VerticalIntegral, PIDVariables.VerticalErrorPrior) * Multiplier), Min, Max);

		PIDVariables.DeltaBL += StabilizationDelta;
		PIDVariables.DeltaBR += StabilizationDelta;
		PIDVariables.DeltaFL += StabilizationDelta;
		PIDVariables.DeltaFR += StabilizationDelta;	
	}
}

void UStabilizationComponentBase::RotationPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max)
{
	if (!AxisDeactivated.bRotation)
	{
		float StabilizationDelta =(PIDFuntion(Drone->DroneMesh->GetPhysicsAngularVelocityInDegrees().Z, Parameters, DeltaTime, PIDVariables.RotationIntegral, PIDVariables.RotationErrorPrior) * Multiplier);
		float StabilizationDeltaInvert = FMath::Clamp(-StabilizationDelta, Min, Max);
		StabilizationDelta = FMath::Clamp(StabilizationDelta, Min, Max);

		PIDVariables.DeltaBL += StabilizationDeltaInvert;
		PIDVariables.DeltaBR += StabilizationDelta;
		PIDVariables.DeltaFL += StabilizationDelta;
		PIDVariables.DeltaFR += StabilizationDeltaInvert;
	}
}

void UStabilizationComponentBase::FrontBacklPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max, FSmoothParametersPID SmoothParameters)
{
	float LocalMultiplier = Multiplier;
	float CurrentValue = (Drone->GetActorRotation().UnrotateVector(Drone->GetVelocity()).X) / SmoothParameters.VelocityDivider + Drone->GetActorRotation().Pitch;
	if (AxisDeactivated.bFrontBack)
	{
		if (abs(Drone->GetActorRotation().Pitch) > SmoothParameters.Interval1 && abs(Drone->GetActorRotation().Pitch) < SmoothParameters.Interval2)
		{
			LocalMultiplier = ((abs(Drone->GetActorRotation().Pitch) - SmoothParameters.SubtrahendDetlaChanger) / SmoothParameters.DividerDetlaChanger) * Multiplier;
		}
		else if (abs(Drone->GetActorRotation().Pitch) > SmoothParameters.Interval2)
		{
			LocalMultiplier = Multiplier;
		}
		else
		{
			return;
		}
	}

	float StabilizationDelta = (PIDFuntion(CurrentValue, Parameters, DeltaTime, PIDVariables.FrontBackIntegral, PIDVariables.FrontBackErrorPrior) * LocalMultiplier);
	float StabilizationDeltaInvert = FMath::Clamp(-StabilizationDelta, Min, Max);
	StabilizationDelta = FMath::Clamp(StabilizationDelta, Min, Max);

	PIDVariables.DeltaBL += StabilizationDeltaInvert;
	PIDVariables.DeltaBR += StabilizationDeltaInvert;
	PIDVariables.DeltaFL += StabilizationDelta;
	PIDVariables.DeltaFR += StabilizationDelta;

}


void UStabilizationComponentBase::LeftRightPID(ADroneBase* Drone, float DeltaTime, FStabilizationParametersPID Parameters, float Multiplier, float Min, float Max, FSmoothParametersPID SmoothParameters)
{
	float LocalMultiplier = Multiplier;
	float CurrentValue = (Drone->GetActorRotation().UnrotateVector(Drone->GetVelocity()).Y) / SmoothParameters.VelocityDivider + Drone->GetActorRotation().Roll;
	if (AxisDeactivated.bLeftRight)
	{
		if (abs(Drone->GetActorRotation().Roll) > SmoothParameters.Interval1 && abs(Drone->GetActorRotation().Roll) < SmoothParameters.Interval2)
		{
			LocalMultiplier = ((abs(Drone->GetActorRotation().Roll) - SmoothParameters.SubtrahendDetlaChanger) / SmoothParameters.DividerDetlaChanger) * Multiplier;
		}
		else if (abs(Drone->GetActorRotation().Roll) > SmoothParameters.Interval2)
		{
			LocalMultiplier = Multiplier;
		}
		else
		{
			return;
		}
	}

	float StabilizationDelta = (PIDFuntion(CurrentValue, Parameters, DeltaTime, PIDVariables.LeftRightIntegral, PIDVariables.LeftRightErrorPrior) * LocalMultiplier);
	float StabilizationDeltaInvert = FMath::Clamp(-StabilizationDelta, Min, Max);
	StabilizationDelta = FMath::Clamp(StabilizationDelta, Min, Max);

	PIDVariables.DeltaBL += StabilizationDelta;
	PIDVariables.DeltaBR += StabilizationDeltaInvert;
	PIDVariables.DeltaFL += StabilizationDelta;
	PIDVariables.DeltaFR += StabilizationDeltaInvert;
}


void UStabilizationComponentBase::ApplyPID(ADroneBase* Drone, float Scale, float Min, float Max)
{
	Drone->FL_TickPower = FMath::Clamp(Drone->FL_TickPower + PIDVariables.DeltaFL * Scale, Min, Max);
	Drone->FR_TickPower = FMath::Clamp(Drone->FR_TickPower + PIDVariables.DeltaFR * Scale, Min, Max);
	Drone->BL_TickPower = FMath::Clamp(Drone->BL_TickPower + PIDVariables.DeltaBL * Scale, Min, Max);
	Drone->BR_TickPower = FMath::Clamp(Drone->BR_TickPower + PIDVariables.DeltaBR * Scale, Min, Max);

	PIDVariables.DeltaBL = 0.0f;
	PIDVariables.DeltaBR = 0.0f;
	PIDVariables.DeltaFL = 0.0f;
	PIDVariables.DeltaFR = 0.0f;
}

void UStabilizationComponentBase::AllAxisML(ADroneBase* Drone, EActions Axis, float FL, float FR, float BL, float BR, float Multiplier, float Min, float Max, FSmoothParametersML SmoothParameters)
{
	bool bIsDeactivated = false;
	float MultiplierParameter = 0.0f;
	switch (Axis)
	{
	case EActions::Vertical:
		bIsDeactivated = AxisDeactivated.bVertical;
		break;
	case EActions::Rotation:
		bIsDeactivated = AxisDeactivated.bRotation;
		break;
	case EActions::LeftRight:
		bIsDeactivated = AxisDeactivated.bLeftRight || AxisDeactivated.bVertical;
		break;
	case EActions::FrontBack:
		bIsDeactivated = AxisDeactivated.bFrontBack || AxisDeactivated.bVertical;
		break;
	}

	MultiplierParameter = !bIsDeactivated ? 1.0f : CheckRotationML(Drone, Axis, SmoothParameters.Divider, SmoothParameters.Min, SmoothParameters.Max);

	float MultiplierLocal = Multiplier * MultiplierParameter;
	MLVariables.DeltaFL = FMath::Clamp(FL * MultiplierLocal + MLVariables.DeltaFL, Min, Max);
	MLVariables.DeltaFR = FMath::Clamp(FR * MultiplierLocal + MLVariables.DeltaFR, Min, Max);
	MLVariables.DeltaBL = FMath::Clamp(BL * MultiplierLocal + MLVariables.DeltaBL, Min, Max);
	MLVariables.DeltaBR = FMath::Clamp(BR * MultiplierLocal + MLVariables.DeltaBR, Min, Max);
}

float UStabilizationComponentBase::CheckRotationML(ADroneBase* Drone, EActions Axis, float Divider, float Min, float Max)
{
	float MultiplierParameter = 0.0f;
	if (Axis == EActions::FrontBack)
	{
		if (abs(Drone->GetActorRotation().Pitch) > Min && abs(Drone->GetActorRotation().Pitch) < Max)
		{
			MultiplierParameter = (abs(Drone->GetActorRotation().Pitch) - Min) / Divider;
		}
		else if (abs(Drone->GetActorRotation().Pitch) > Max)
		{
			MultiplierParameter = 1.0f;
		}
	}
	else if (Axis == EActions::LeftRight)
	{
		if (abs(Drone->GetActorRotation().Roll) > Min && abs(Drone->GetActorRotation().Roll) < Max)
		{
			MultiplierParameter = (abs(Drone->GetActorRotation().Roll) - Min) / Divider;
		}
		else if (abs(Drone->GetActorRotation().Roll) > Max)
		{
			MultiplierParameter = 1.0f;
		}
	}

	return  MultiplierParameter;
}

void UStabilizationComponentBase::ApplyML(ADroneBase* Drone, float Scale, float Delta, float Min, float Max)
{
	Drone->FL_TickPower = FMath::Clamp(FMath::Clamp(Drone->FL_TickPower + MLVariables.DeltaFL * Scale, Drone->FL_TickPower - Delta, Drone->FL_TickPower + Delta), Min, Max);
	Drone->FR_TickPower = FMath::Clamp(FMath::Clamp(Drone->FR_TickPower + MLVariables.DeltaFR * Scale, Drone->FR_TickPower - Delta, Drone->FR_TickPower + Delta), Min, Max);
	Drone->BL_TickPower = FMath::Clamp(FMath::Clamp(Drone->BL_TickPower + MLVariables.DeltaBL * Scale, Drone->BL_TickPower - Delta, Drone->BL_TickPower + Delta), Min, Max);
	Drone->BR_TickPower = FMath::Clamp(FMath::Clamp(Drone->BR_TickPower + MLVariables.DeltaBR * Scale, Drone->BR_TickPower - Delta, Drone->BR_TickPower + Delta), Min, Max);

	MLVariables.DeltaFL = 0.0f;
	MLVariables.DeltaFR = 0.0f;
	MLVariables.DeltaBL = 0.0f;
	MLVariables.DeltaBR = 0.0f;
}

void UStabilizationComponentBase::ResetAxisDeactivated()
{
	AxisDeactivated.bFrontBack = false;
	AxisDeactivated.bLeftRight = false;
	AxisDeactivated.bRotation = false;
	AxisDeactivated.bVertical = false;
}


// Called every frame
void UStabilizationComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStabilizationComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStabilizationComponentBase, PIDVariables);
	DOREPLIFETIME(UStabilizationComponentBase, MLVariables);
	DOREPLIFETIME(UStabilizationComponentBase, AxisDeactivated);
}

