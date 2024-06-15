// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstanceBase.h"
#include "AbilitySystemGlobals.h"


void UGameInstanceBase::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
