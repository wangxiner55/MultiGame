// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../Interfaces/CharacterInfoInterface.h"
#include "PlayerAnimInstance.generated.h"


/**
 * 
 */


UCLASS()
class BLASTER_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:



	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FVector MovementInput;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsMoving;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool IsAiming;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool HasMovementInput;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float MovementInputAmount;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float AimYawRate;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FRotator AimingRotation;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	EGait Gait;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	EStance Stance;
	
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	EOverlayState OverlayState;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	EPlayerWeaponState WeaponState;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsEquipWeapon;


private:
	struct FCharacterData  CharacterData;
	struct FCharacterState CharacterState;

public:

	void GetPlayerCharacterInfo();

	void GetPlayerCharacterState();



private:

	APawn* Owner;
	ICharacterInfoInterface* playerInterface; 
	
private:
	
	// Update Data from CharacterBP
	void UpdatePlayerBaseInfo();
	void SetAnimPlayerBaseValue();
	void SetAnimPlayerBaseState();
	FVector2D InterpLeanAmount(FVector2D Current, FVector2D Target, float InterpSpeed, float DeltaTime);

	void UpdateMovementValue();
	FVector CalculateRelativeAccelerationAmount();



private:

	float DeltaT;
	float LeanInterpSpeed;
	FVector2D LeanAmount;
	ACharacter* Character;
	class UCharacterMovementComponent* CharacterMovement;
};


