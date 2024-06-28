// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class USceneComponent;

UCLASS(Blueprintable)
class DRONESTABILIZATION_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> RootComponentCustom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
