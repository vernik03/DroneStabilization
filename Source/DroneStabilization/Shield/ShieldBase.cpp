// Fill out your copyright notice in the Description page of Project Settings.


#include "Shield/ShieldBase.h"


// Sets default values
AShieldBase::AShieldBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponentCustom = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComponentCustom;


	GeneratorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GeneratorMesh"));
	GeneratorMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AShieldBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShieldBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

