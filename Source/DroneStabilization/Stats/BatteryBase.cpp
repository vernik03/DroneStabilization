// Fill out your copyright notice in the Description page of Project Settings.


#include "Stats/BatteryBase.h"


// Sets default values
ABatteryBase::ABatteryBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	RootComponentCustom = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComponentCustom;


	BatteryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BatteryMesh"));
	BatteryMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABatteryBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABatteryBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

