// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DroneDataTypes.h"
#include "DroneDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DRONESTABILIZATION_API UDroneDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	FDroneData DroneData;
	
};
