// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBase.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ADroneBase::ADroneBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DroneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	RootComponent = DroneMesh;

	PhysicsThrusterBR = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("PhysicsThrusterBR"));
	PhysicsThrusterBR->SetupAttachment(DroneMesh);

	PhysicsThrusterBL = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("PhysicsThrusterBL"));
	PhysicsThrusterBL->SetupAttachment(DroneMesh);

	PhysicsThrusterFL = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("PhysicsThrusterFL"));
	PhysicsThrusterFL->SetupAttachment(DroneMesh);

	PhysicsThrusterFR = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("PhysicsThrusterFR"));
	PhysicsThrusterFR->SetupAttachment(DroneMesh);
}

// Called when the game starts or when spawned
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	ActiviteAllEngines(false);
	StartLocation = GetActorLocation();
	DroneMesh->SetCenterOfMass(CalculateCenterOfMass());
}

void ADroneBase::ActiviteAllEngines(bool bActivate)
{
	PhysicsThrusterFR->SetActive(bActivate);
	PhysicsThrusterFL->SetActive(bActivate);
	PhysicsThrusterBL->SetActive(bActivate);
	PhysicsThrusterBR->SetActive(bActivate);

	PhysicsThrusterFR->ThrustStrength = ThrustStrengthBase * bActivate;
	PhysicsThrusterFL->ThrustStrength = ThrustStrengthBase * bActivate;
	PhysicsThrusterBL->ThrustStrength = ThrustStrengthBase * bActivate;
	PhysicsThrusterBR->ThrustStrength = ThrustStrengthBase * bActivate;
}

FVector ADroneBase::CalculateCenterOfMass()
{
	const FVector TempCenterOfMass = (PhysicsThrusterFR->GetComponentLocation() + PhysicsThrusterBR->GetComponentLocation()) / 2 - DroneMesh->GetCenterOfMass();
	const FVector CenterOfMass = DroneMesh->GetComponentRotation().GetInverse().RotateVector(TempCenterOfMass);
	return FVector(CenterOfMass.X / 100, 0, 0);
}

void ADroneBase::ResetDrone()
{
	DroneMesh->SetSimulatePhysics(false);
	DroneMesh->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	DroneMesh->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
	SetActorLocation(StartLocation);
	SetActorRotation(FRotator(0, 0, 0));
	DroneMesh->ResetAllBodiesSimulatePhysics();
	OnResetDroneInBP();
	FR_TickPower = 0.0f;
	FL_TickPower = 0.0f;
	BL_TickPower = 0.0f;
	BR_TickPower = 0.0f;
	PhysicsThrusterFR->ThrustStrength = ThrustStrengthBase + FR_TickPower;
	PhysicsThrusterFL->ThrustStrength = ThrustStrengthBase + FL_TickPower;
	PhysicsThrusterBL->ThrustStrength = ThrustStrengthBase + BL_TickPower;
	PhysicsThrusterBR->ThrustStrength = ThrustStrengthBase + BR_TickPower;
	DroneMesh->SetSimulatePhysics(true);
	if (bIsAgent)
	{
		OnAddRandomImpulse();
	}
}

void ADroneBase::VerticalMovement(float ActionValue, float Magnitude)
{
	float LocalActionValue = FMath::Clamp(ActionValue, -10.0f, 3.0f);
	LocalActionValue = LocalActionValue * Magnitude;
	FR_TickPower = FMath::Clamp(LocalActionValue + FR_TickPower, -ThrustStrengthBase, 10000);
	FL_TickPower = FMath::Clamp(LocalActionValue + FL_TickPower, -ThrustStrengthBase, 10000);
	BL_TickPower = FMath::Clamp(LocalActionValue + BL_TickPower, -ThrustStrengthBase, 10000);
	BR_TickPower = FMath::Clamp(LocalActionValue + BR_TickPower, -ThrustStrengthBase, 10000);
}

void ADroneBase::RotationMovement(float ActionValue, float Magnitude)
{
	const float LocalActionValue = FMath::Clamp(ActionValue, -3.0f, 3.0f);
	FR_TickPower += (LocalActionValue * (-Magnitude));
	FL_TickPower += (LocalActionValue * Magnitude);
	BL_TickPower += (LocalActionValue * (-Magnitude));
	BR_TickPower += (LocalActionValue * Magnitude);
}

void ADroneBase::FrontBackMovement(float ActionValue, float Magnitude, float Limiter)
{
	const float LocalActionValue = abs(FMath::Clamp(ActionValue, -Limiter, Limiter));
	if (ActionValue >= 0)
	{
		FR_TickPower += (LocalActionValue * Magnitude);
		FL_TickPower += (LocalActionValue * Magnitude);		
	}
	else
	{
		BL_TickPower += (LocalActionValue * Magnitude);
		BR_TickPower += (LocalActionValue * Magnitude);
	}
}

void ADroneBase::LeftRightMovement(float ActionValue, float Magnitude, float Limiter)
{
	const float LocalActionValue = abs(FMath::Clamp(ActionValue, -Limiter, Limiter));
	if (ActionValue >= 0)
	{
		FL_TickPower += (LocalActionValue * Magnitude);
		BL_TickPower += (LocalActionValue * Magnitude);		
	}
	else
	{
		FR_TickPower += (LocalActionValue * Magnitude);
		BR_TickPower += (LocalActionValue * Magnitude);
	}
}

// Called every frame
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsEnginesActivated)
	{
		PhysicsThrusterFR->ThrustStrength = ThrustStrengthBase + FR_TickPower;
		PhysicsThrusterFL->ThrustStrength = ThrustStrengthBase + FL_TickPower;
		PhysicsThrusterBL->ThrustStrength = ThrustStrengthBase + BL_TickPower;
		PhysicsThrusterBR->ThrustStrength = ThrustStrengthBase + BR_TickPower;

		float TorqueFR = (FR_TickPower * AdditionalRotationPowerFirst + ThrustStrengthBase) * -AdditionalRotationPowerSecond;
		float TorqueFL = (FL_TickPower * AdditionalRotationPowerFirst + ThrustStrengthBase) * AdditionalRotationPowerSecond;
		float TorqueBL = (BL_TickPower * AdditionalRotationPowerFirst + ThrustStrengthBase) * -AdditionalRotationPowerSecond;
		float TorqueBR = (BR_TickPower * AdditionalRotationPowerFirst + ThrustStrengthBase) * AdditionalRotationPowerSecond;

		DroneMesh->AddTorqueInDegrees(FVector(0.0f, 0.0f, TorqueFR), "Fan_FR_end", true);
		DroneMesh->AddTorqueInDegrees(FVector(0.0f, 0.0f, TorqueFL), "Fan_FL_end", true);
		DroneMesh->AddTorqueInDegrees(FVector(0.0f, 0.0f, TorqueBL), "Fan_BL_end", true);
		DroneMesh->AddTorqueInDegrees(FVector(0.0f, 0.0f, TorqueBR), "Fan_BR_end", true);

		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterFR->ThrustStrength/DynamicForceFeedbackDivider, 0.1f, false, false, false, true, EDynamicForceFeedbackAction::Start);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterFL->ThrustStrength/DynamicForceFeedbackDivider, 0.1f, false, true, false, false, EDynamicForceFeedbackAction::Start);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterBL->ThrustStrength/DynamicForceFeedbackDivider, 0.1f, true, false, false, false, EDynamicForceFeedbackAction::Start);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterBR->ThrustStrength/DynamicForceFeedbackDivider, 0.1f, false, false, true, false, EDynamicForceFeedbackAction::Start);

		FR_TickPower= 0.0f;
		FL_TickPower= 0.0f;
		BL_TickPower= 0.0f;
		BR_TickPower= 0.0f;
	}
}

// Called to bind functionality to input
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


