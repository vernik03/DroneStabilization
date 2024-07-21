
// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneBase.h"
#include "StabilizationComponentBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AttributeSets/AttributeSetBase.h"
#include "DataAssets/DroneDataAsset.h"
#include "AbilitySystem/Components/AbilitySystemComponentBase.h"



// Sets default values
ADroneBase::ADroneBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
    AActor::SetReplicateMovement(true);

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

	StabilizationComponent = CreateDefaultSubobject<UStabilizationComponentBase>(TEXT("StabilizationComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSet"));

}

// Called when the game starts or when spawned
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	ActiviteAllEngines(false);
	StartLocation = GetActorLocation();
	DroneMesh->SetCenterOfMass(CalculateCenterOfMass());
}

bool ADroneBase::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
	FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);

	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		return ActiveGEHandle.WasSuccessfullyApplied();
	}

	return false;
}

UAbilitySystemComponent* ADroneBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FDroneData ADroneBase::GetDroneData() const
{
	return DroneData;
}

void ADroneBase::SetDroneData(const FDroneData& InDroneData)
{	
	DroneData = InDroneData;
	InitFromDroneData(DroneData);
}

void ADroneBase::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : DroneData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void ADroneBase::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto DefaultEffect : DroneData.Effects)
		{
			ApplyGameplayEffectToSelf(DefaultEffect, EffectContext);
		}
	}
}

void ADroneBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	GiveAbilities();
	ApplyStartupEffects();
}

void ADroneBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ADroneBase::OnRep_DroneData()
{

	InitFromDroneData(DroneData, true);
}

void ADroneBase::InitFromDroneData(const FDroneData InDroneData, bool bFromReplication)
{
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
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewLocation.Z + 200;
	SetActorLocation(NewLocation);	
	
	SetActorRotation(FRotator(0, 0, 0));

	DroneMesh->ResetAllBodiesSimulatePhysics();
	StabilizationComponent->ResetAxisDeactivated();
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

void ADroneBase::VerticalMovement(float ActionValue, float Magnitude, float Limiter)
{
	if (!ActionsUsed.bVertical)
	{
		float LocalActionValue = FMath::Clamp(ActionValue, -Limiter, Limiter);
		LocalActionValue = LocalActionValue * Magnitude;
		FR_TickPower = FMath::Clamp(LocalActionValue + FR_TickPower, -ThrustStrengthBase, 10000);
		FL_TickPower = FMath::Clamp(LocalActionValue + FL_TickPower, -ThrustStrengthBase, 10000);
		BL_TickPower = FMath::Clamp(LocalActionValue + BL_TickPower, -ThrustStrengthBase, 10000);
		BR_TickPower = FMath::Clamp(LocalActionValue + BR_TickPower, -ThrustStrengthBase, 10000);

		ActionsUsed.bVertical = true;
	}
}

void ADroneBase::RotationMovement(float ActionValue, float Magnitude, float Limiter)
{
	if (!ActionsUsed.bRotation)
	{
		const float LocalActionValue = FMath::Clamp(ActionValue, -Limiter, Limiter);
		FR_TickPower += (LocalActionValue * (-Magnitude));
		FL_TickPower += (LocalActionValue * Magnitude);
		BL_TickPower += (LocalActionValue * (-Magnitude));
		BR_TickPower += (LocalActionValue * Magnitude);

		ActionsUsed.bRotation = true;
	}

}

void ADroneBase::FrontBackMovement(float ActionValue, float Magnitude, float Limiter)
{
	DesiredValues.FrontBack = FMath::Clamp(ActionValue, -Limiter, Limiter) * Magnitude;
}

void ADroneBase::LeftRightMovement(float ActionValue, float Magnitude, float Limiter)
{
	DesiredValues.LeftRight = FMath::Clamp(ActionValue, -Limiter, Limiter) * Magnitude;
}

void ADroneBase::SetDesiredValues(EActions Axis, float Value)
{
	switch (Axis)
	{
	case EActions::Vertical:
		DesiredValues.Vertical = Value;
		break;
	case EActions::Rotation:
		DesiredValues.Rotation = Value;
		break;
	case EActions::LeftRight:
		DesiredValues.LeftRight = Value;
		break;
	case EActions::FrontBack:
		DesiredValues.FrontBack = Value;
		break;
	default:
		break;
	}
}

// Called every frame
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	if (bIsEnginesActivated)
	{

		StabilizationComponent->CustomPreTickEvent(DeltaTime);

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

		FR_TickPower= 0.0f;
		FL_TickPower= 0.0f;
		BL_TickPower= 0.0f;
		BR_TickPower= 0.0f;

		ActionsUsed.bVertical = false;
		ActionsUsed.bRotation = false;
		ActionsUsed.bFrontBack = false;
		ActionsUsed.bLeftRight = false;

		StabilizationComponent->CustomPostTickEvent(DeltaTime);
	}
	if (bIsEnginesActivated && !HasAuthority())
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterFR->ThrustStrength / DynamicForceFeedbackDivider, 0.1f, false, false, false, true, EDynamicForceFeedbackAction::Start);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterFL->ThrustStrength / DynamicForceFeedbackDivider, 0.1f, false, true, false, false, EDynamicForceFeedbackAction::Start);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterBL->ThrustStrength / DynamicForceFeedbackDivider, 0.1f, true, false, false, false, EDynamicForceFeedbackAction::Start);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayDynamicForceFeedback(PhysicsThrusterBR->ThrustStrength / DynamicForceFeedbackDivider, 0.1f, false, false, true, false, EDynamicForceFeedbackAction::Start);
	}
}

// Called to bind functionality to input
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADroneBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(DroneDataAsset))
	{
		SetDroneData(DroneDataAsset->DroneData);
	}
}

void ADroneBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADroneBase, bIsEnginesActivated);
	DOREPLIFETIME(ADroneBase, FR_TickPower);
	DOREPLIFETIME(ADroneBase, FL_TickPower);
	DOREPLIFETIME(ADroneBase, BL_TickPower);
	DOREPLIFETIME(ADroneBase, BR_TickPower);
	DOREPLIFETIME(ADroneBase, StartLocation);
	DOREPLIFETIME(ADroneBase, bIsStabilizationEnabled);
	DOREPLIFETIME(ADroneBase, bIsNeuralStabilization);
	DOREPLIFETIME(ADroneBase, DroneData);
	DOREPLIFETIME(ADroneBase, DesiredValues);
}

