// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.h"
#include "../Data/Enum/GameEnum.h"
#include "CharacterInfoInterface.generated.h"




// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLASTER_API ICharacterInfoInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:



	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//	float Speed();

	virtual FCharacterData GetCharacterInfo() = 0;
	virtual FCharacterState GetCharacterState() = 0;
	virtual bool IsCharacterWeaponEquiped() = 0;


	virtual void SetMovementState(const enum EMovementState& state) = 0;
	virtual void SetGaitState(const enum EGait& state) = 0;
	virtual void SetStanceState(const enum EStance& state) = 0;
	virtual void SetOverlayState(const enum EOverlayState& state) = 0;

};


UENUM(BlueprintType)
	enum class EMovementState : uint8
{
	None UMETA(DisplayName = "None"),
	Grounded UMETA(DisplayName = "Grounded"),
	InAir UMETA(DisplayName = "InAir"), 
	Mantling UMETA(DisplayName = "Mantling"), 
	Ragdoll  UMETA(DisplayName = "Ragdoll")
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Sprinting UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	Standing UMETA(DisplayName = "Standing"),
	Crouching UMETA(DisplayName = "Crouching"),
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	Default UMETA(DisplayName = "Default"),
	Gun UMETA(DisplayName = "Gun"),
};

USTRUCT()
struct FCharacterData
{
	GENERATED_BODY()
public:
	FVector Velocity;
	FVector Acceleration;
	FVector MovementInput;

	FRotator AimingRotation;

	float Speed;
	float MovementInputAmount;
	float AimYawRate;

	bool bIsAiming;
	bool bIsMoving;
	bool bHasMovementInput;
	bool bIsEquipedWeapon;

};

USTRUCT()
struct FCharacterState
{
	GENERATED_BODY()
public:
	EMovementState MovementState;
	EGait Gait;
	EStance Stance;
	EOverlayState OverlayState;
};