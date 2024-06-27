// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponentBase.generated.h"

class USkeletalMeshComponent;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DRONESTABILIZATION_API UWeaponComponentBase : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponentBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Mesh" )
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMesh> WeaponMeshAsset;
*/


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
