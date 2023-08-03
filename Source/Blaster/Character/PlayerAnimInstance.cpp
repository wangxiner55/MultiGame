// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "../Debug/DebugUtility.h"
#include "PlayerCharacterBase.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Owner = TryGetPawnOwner();
	playerInterface = Cast<APlayerCharacterBase>(Owner);
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{


}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{

	GetPlayerCharacterInfo();
	GetPlayerCharacterState();
	SetAnimState();
	SetAnimValue();



}



void UPlayerAnimInstance::GetPlayerCharacterInfo()
{

	if(playerInterface)
	CharacterData = playerInterface->GetCharacterInfo();

}

void UPlayerAnimInstance::GetPlayerCharacterState()
{
	if (playerInterface)
	CharacterState = playerInterface->GetCharacterState();
}

void UPlayerAnimInstance::SetAnimValue()
{
	Velocity				=		CharacterData.Velocity;
	Speed					=		CharacterData.Speed;
	Acceleration			=		CharacterData.Acceleration;
	MovementInput			=		CharacterData.MovementInput;
	IsMoving				=		CharacterData.bIsMoving;
	IsAiming				=		CharacterData.bIsAiming;
	HasMovementInput		=		CharacterData.bHasMovementInput;
	MovementInputAmount		=		CharacterData.MovementInputAmount;
	AimYawRate				=		CharacterData.AimYawRate;
	AimingRotation			=		CharacterData.AimingRotation;
	bIsEquipWeapon			=		CharacterData.bIsEquipedWeapon;

}

void UPlayerAnimInstance::SetAnimState()
{
	MovementState			 =			CharacterState.MovementState;
	Stance					 =			CharacterState.Stance;
	OverlayState			 =			CharacterState.OverlayState;
	Gait					 =			CharacterState.Gait;
}


