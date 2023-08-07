// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "../Debug/DebugUtility.h"
#include "PlayerCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Owner = TryGetPawnOwner();
	
	Character = Cast<ACharacter>(Owner);
	playerInterface = Cast<ICharacterInfoInterface>(Character);

	
	
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	CharacterMovement = Character->GetCharacterMovement();
	LeanAmount = FVector2D(0.0f);
	LeanInterpSpeed = 4.0f;
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	DeltaT = DeltaSeconds;
	UpdatePlayerBaseInfo();

	UpdateMovementValue();

}

void UPlayerAnimInstance::UpdatePlayerBaseInfo()
{
	GetPlayerCharacterInfo();
	GetPlayerCharacterState();
	SetAnimPlayerBaseState();
	SetAnimPlayerBaseValue();

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

void UPlayerAnimInstance::SetAnimPlayerBaseValue()
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

void UPlayerAnimInstance::SetAnimPlayerBaseState()
{
	MovementState			 =			CharacterState.MovementState;
	Stance					 =			CharacterState.Stance;
	OverlayState			 =			CharacterState.OverlayState;
	Gait					 =			CharacterState.Gait;
}



void UPlayerAnimInstance::UpdateMovementValue()
{
	FVector AccAmount = CalculateRelativeAccelerationAmount();
	FVector2D TargetLeanAmount = FVector2D(AccAmount.Y, AccAmount.X);
	LeanAmount = InterpLeanAmount(LeanAmount, TargetLeanAmount, LeanInterpSpeed, DeltaT);
}

FVector UPlayerAnimInstance::CalculateRelativeAccelerationAmount()
{
	bool bIsSameless = UKismetMathLibrary::Dot_VectorVector(Acceleration, Velocity) > 0.0f ? true : false;
	if (bIsSameless)
	{
		float MaxAcc = CharacterMovement->GetMaxAcceleration();
		return UKismetMathLibrary::LessLess_VectorRotator(
			UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxAcc) / MaxAcc,
			Character->GetActorRotation());
	}
	else
	{
		float MaxAcc = CharacterMovement->GetMaxBrakingDeceleration();
		return UKismetMathLibrary::LessLess_VectorRotator(
			UKismetMathLibrary::Vector_ClampSizeMax(Acceleration, MaxAcc) / MaxAcc,
			Character->GetActorRotation());
	}
}



FVector2D UPlayerAnimInstance::InterpLeanAmount(FVector2D Current, FVector2D Target, float InterpSpeed, float DeltaTime)
{
	return FVector2D(
		UKismetMathLibrary::FInterpTo(Current.X, Target.X, DeltaTime, InterpSpeed),
		UKismetMathLibrary::FInterpTo(Current.Y, Target.Y, DeltaTime, InterpSpeed));
}
