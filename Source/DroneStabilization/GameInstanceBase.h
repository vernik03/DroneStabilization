// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class DRONESTABILIZATION_API UGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	
	
};
