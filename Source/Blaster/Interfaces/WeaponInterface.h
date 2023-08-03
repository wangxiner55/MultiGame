// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLASTER_API IWeaponInterface
{
	GENERATED_BODY()

public:
	virtual FWeaponInfo GetWeaponInfo() = 0;
	virtual void SetWeaponState(const EPlayerWeaponState& State) = 0;
	virtual EPlayerWeaponState GetWeaponState() = 0;
	



};



UENUM(BlueprintType)
enum class EPlayerWeaponState : uint8
{
	EWS_None UMETA(DisplayName = "None"),
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};



UENUM(BlueprintType)
enum class EWeaponAttr : uint8
{
	EWT_Kendo UMETA(DisplayName = "Kendo"),
	EWT_Gun UMETA(DisplayName = "Gun"),
	EWT_staff UMETA(DisplayName = "staff"),
};

USTRUCT()
struct FWeaponInfo
{
	GENERATED_BODY()

public:
	float Damage;
	bool bCanAiming;
	EWeaponAttr WeaponType;

};